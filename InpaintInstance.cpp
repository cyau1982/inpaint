#include <random>
#include <pcl/AutoViewLock.h>
#include <pcl/Console.h>
#include <pcl/FFTConvolution.h>
#include <pcl/IntegerResample.h>
#include <pcl/MorphologicalTransformation.h>
#include <pcl/MultiscaleLinearTransform.h>
#include <pcl/PixelInterpolation.h>
#include <pcl/Resample.h>
#include <pcl/StandardStatus.h>
#include <pcl/VariableShapeFilter.h>
#include <pcl/View.h>

#include "InpaintInstance.h"
#include "InpaintParameters.h"

namespace pcl
{

template <class P>
class InpaintThread : public Thread
{
public:
    typedef void(*LineProcessFunc)(InpaintInstance* inpaint, ReferenceArray<GenericImage<P>>&, GenericImage<P>&, int, int);

    InpaintThread(int id, LineProcessFunc lineProcessFunc, const AbstractImage::ThreadData& data, InpaintInstance* inpaint,
        ReferenceArray<GenericImage<P>>& srcImages, GenericImage<P>& dstImage,
        int channel, int firstRow, int endRow)
        : m_id(id)
        , m_lineProcessFunc(lineProcessFunc)
        , m_data(data)
        , m_inpaint(inpaint)
        , m_srcImages(srcImages)
        , m_dstImage(dstImage)
        , m_channel(channel)
        , m_firstRow(firstRow)
        , m_endRow(endRow)
        , m_threadErrorMsg("")
    {
    }

    void Run() override
    {
        INIT_THREAD_MONITOR();
        try {
            for (int y = m_firstRow; y < m_endRow; y++) {
                m_lineProcessFunc(m_inpaint, m_srcImages, m_dstImage, y, m_channel);
                UPDATE_THREAD_MONITOR(65536);
            }
        }
        catch (...) {
            volatile AutoLock lock(m_data.mutex);
            try {
                throw;
            }
            catch (Exception & x) {
                m_threadErrorMsg = x.Message();
            }
            catch (std::bad_alloc&) {
                m_threadErrorMsg = "Out of memory";
            }
            catch (...) {
                m_threadErrorMsg = "Unknown error";
            }
        }
    }

    static void dispatch(LineProcessFunc lineProcessFunc, InpaintInstance* inpaint,
        ReferenceArray<GenericImage<P>>& srcImages, GenericImage<P>& dstImage, int channel)
    {
        Array<size_type> L = Thread::OptimalThreadLoads(dstImage.Height(), 1, PCL_MAX_PROCESSORS);
        ReferenceArray<InpaintThread> threads;
        AbstractImage::ThreadData data(dstImage, dstImage.NumberOfPixels());
        for (int i = 0, n = 0; i < int(L.Length()); n += int(L[i++]))
            threads << new InpaintThread(i, lineProcessFunc, data, inpaint, srcImages, dstImage, channel, n, n + int(L[i]));
        AbstractImage::RunThreads(threads, data);
        for (InpaintThread& t : threads)
            if (t.m_threadErrorMsg != "")
                throw Error(t.m_threadErrorMsg);
        threads.Destroy();
        dstImage.Status() = data.status;
    }

private:
    int m_id;
    LineProcessFunc m_lineProcessFunc;
    const AbstractImage::ThreadData& m_data;
    InpaintInstance* m_inpaint;
    ReferenceArray<GenericImage<P>>& m_srcImages;
    GenericImage<P>& m_dstImage;
    int m_channel;
    int m_firstRow;
    int m_endRow;
    String m_threadErrorMsg;
};

InpaintInstance::InpaintInstance(const MetaProcess* m)
    : ProcessImplementation(m)
    , blackPoint(TheInpaintBlackPointParameter->DefaultValue())
    , precision(TheInpaintPrecisionParameter->DefaultValue())
{
}

InpaintInstance::InpaintInstance(const InpaintInstance& x)
    : ProcessImplementation(x)
{
    Assign(x);
}

void InpaintInstance::Assign(const ProcessImplementation& p)
{
    const InpaintInstance* x = dynamic_cast<const InpaintInstance*>(&p);
    if (x != nullptr) {
        blackPoint = x->blackPoint;
        precision = x->precision;
    }
}

bool InpaintInstance::IsHistoryUpdater(const View& view) const
{
    return true;
}

UndoFlags InpaintInstance::UndoMode(const View&) const
{
    return UndoFlag::PixelData;
}

bool InpaintInstance::CanExecuteOn(const View& view, pcl::String& whyNot) const
{
    if (view.Image().IsComplexSample())
    {
        whyNot = "Inpaint cannot be executed on complex images.";
        return false;
    } else if (!view.Image().IsFloatSample()) {
        whyNot = "Inpaint can only be executed on float images.";
        return false;
    }

    return true;
}

bool InpaintInstance::ExecuteOn(View& view)
{
    AutoViewLock lock(view);

    StandardStatus status;
    Console console;

    console.EnableAbort();

    ImageVariant image = view.Image();

    if (image.IsComplexSample() || !view.Image().IsFloatSample())
        return false;

    image.SetStatusCallback(&status);
    image.Status().Initialize("Running inpainting", 1);

    // Inpaint
    ImageVariant src;
    src.CopyImage(image);
    src.EnsureUniqueImage();
    src.SetStatusCallback(nullptr);
    if (image.BitsPerSample() == 32) {
        ReferenceArray<GenericImage<FloatPixelTraits>> input;
        input << &static_cast<Image&>(*src);
        for (int c = 0; c < image.NumberOfChannels(); c++) {
            InpaintThread<FloatPixelTraits>::dispatch(doInpaint<FloatPixelTraits>, this, input, static_cast<Image&>(*image), c);
            image.Status() += 1;
        }
    } else if (image.BitsPerSample() == 64) {
        ReferenceArray<GenericImage<DoublePixelTraits>> input;
        input << &static_cast<DImage&>(*src);
        for (int c = 0; c < image.NumberOfChannels(); c++) {
            InpaintThread<DoublePixelTraits>::dispatch(doInpaint<DoublePixelTraits>, this, input, static_cast<DImage&>(*image), c);
            image.Status() += 1;
        }
    }

    image.Status() += 1;
    image.Status().Complete();

    return true;
}

void* InpaintInstance::LockParameter(const MetaParameter* p, size_type /*tableRow*/)
{
    return 0;
}

template <class P>
void InpaintInstance::doInpaint(InpaintInstance* inpaint, ReferenceArray<GenericImage<P>>& inputs, GenericImage<P>& output, int y, int channel)
{
    typename P::sample* pOut = output.ScanLine(y, channel);
    GenericImage<P>& input = inputs[0];
    const int n = inpaint->precision;
    const int distance = pcl::Max(output.Width(), output.Height());
    std::minstd_rand rg(output.Height() * channel + y);
    std::uniform_real_distribution<float> ud(-0.5f, 0.5f);

    for (int x = 0; x < output.Width(); x++) {
        typename P::sample in = input(x, y, channel);
        if (in > inpaint->blackPoint) {
            pOut[x] = in;
            continue;
        }
        typename P::sample p = 0.0;
        float w0 = 0.0f;
        for (int i = 0; i < n; i++) {
            float rad = pcl::Pi() * 2.0f * i / n;
            float step_x = pcl::Cos(rad);
            float step_y = pcl::Sin(rad);
            for (int j = 1; j < distance; j = (j < 16) ? j + 1 : j * 1.1f) {
                float w = 1.0f / float(j);
                if (w < w0 * 0.01f)
                    continue;
                float rx = ud(rg) * j * 6.0f / n;
                float ry = ud(rg) * j * 6.0f / n;
                rx = 0.0;
                ry = 0.0;
                int ix = int(x + step_x * j + rx + 0.5f);
                int iy = int(y + step_y * j + ry + 0.5f);
                if (ix < 0)
                    ix = 0;
                else if (ix >= input.Width())
                    ix = input.Width() - 1;
                if (iy < 0)
                    iy = 0;
                else if (iy >= input.Height())
                    iy = input.Height() - 1;
                typename P::sample in = input(ix, iy, channel);
                if (in <= inpaint->blackPoint)
                    continue;
                p += in * w;
                w0 += w;
                break;
            }
        }
        if (w0 > 0.0f)
            pOut[x] = p / w0;
        else
            pOut[x] = 0.0;
    }
}

}	// namespace pcl
