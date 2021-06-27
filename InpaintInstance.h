#ifndef __InpaintInstance_h
#define __InpaintInstance_h

#include <pcl/ProcessImplementation.h>
#include <pcl/MetaParameter.h> // pcl_enum

namespace pcl
{

class InpaintInstance : public ProcessImplementation
{
public:
    InpaintInstance(const MetaProcess*);
    InpaintInstance(const InpaintInstance&);

    void Assign(const ProcessImplementation&) override;
    bool IsHistoryUpdater(const View& v) const override;
    UndoFlags UndoMode(const View&) const override;
    bool CanExecuteOn(const View&, pcl::String& whyNot) const override;
    bool ExecuteOn(View&) override;
    void* LockParameter(const MetaParameter*, size_type tableRow) override;

private:
    float blackPoint;
    int precision;

    template <class P>
    static void doInpaint(InpaintInstance* inpaint, ReferenceArray<GenericImage<P>>& inputs, GenericImage<P>& output, int y, int channel);

    friend class InpaintProcess;
    friend class InpaintInterface;
};

}	// namespace pcl

#endif	// __InpaintInstance_h
