#ifndef __InpaintInterface_h
#define __InpaintInterface_h

#include <pcl/NumericControl.h>
#include <pcl/ProcessInterface.h>
#include <pcl/Sizer.h>

#include "InpaintInstance.h"

namespace pcl {

class InpaintInterface : public ProcessInterface
{
public:
    InpaintInterface();
    virtual ~InpaintInterface();

    IsoString Id() const override;
    MetaProcess* Process() const override;
    IsoString IconImageSVG() const override;
    InterfaceFeatures Features() const override;
    void ApplyInstance() const override;
    void ResetInstance() override;
    bool Launch(const MetaProcess&, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/) override;
    ProcessImplementation* NewProcess() const override;
    bool ValidateProcess(const ProcessImplementation&, pcl::String& whyNot) const override;
    bool RequiresInstanceValidation() const override;
    bool ImportProcess(const ProcessImplementation&) override;

private:
    InpaintInstance instance;

    struct GUIData
    {
        GUIData(InpaintInterface&);

        VerticalSizer   Global_Sizer;
            HorizontalSizer InpaintBlackPoint_Sizer;
                NumericControl  InpaintBlackPoint_NumericControl;
            HorizontalSizer InpaintPrecision_Sizer;
                NumericControl  InpaintPrecision_NumericControl;
    };

    GUIData* GUI = nullptr;

    void UpdateControls();
    void __EditValueUpdated(NumericEdit& sender, double value);

    friend struct GUIData;
};

PCL_BEGIN_LOCAL
extern InpaintInterface* TheInpaintInterface;
PCL_END_LOCAL

}	// namespace pcl

#endif  // __InpaintInterface_h
