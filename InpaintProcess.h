#ifndef __InpaintProcess_h
#define __InpaintProcess_h

#include <pcl/MetaProcess.h>

namespace pcl
{

class InpaintProcess : public MetaProcess
{
public:
    InpaintProcess();

    IsoString Id() const override;
    IsoString Category() const override;
    uint32 Version() const override;
    String Description() const override;
    IsoString IconImageSVG() const override;
    ProcessInterface* DefaultInterface() const override;
    ProcessImplementation* Create() const override;
    ProcessImplementation* Clone(const ProcessImplementation&) const override;
    bool NeedsValidation() const override;
    bool CanProcessCommandLines() const override;
};

PCL_BEGIN_LOCAL
extern InpaintProcess* TheInpaintProcess;
PCL_END_LOCAL

}	// namespace pcl

#endif	// __InpaintProcess_h
