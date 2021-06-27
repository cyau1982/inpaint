#ifndef __InpaintParameters_h
#define __InpaintParameters_h

#include <pcl/MetaParameter.h>

namespace pcl
{

PCL_BEGIN_LOCAL

class InpaintBlackPoint : public MetaFloat
{
public:
    InpaintBlackPoint(MetaProcess*);

    IsoString Id() const override;
    int Precision() const override;
    double MinimumValue() const override;
    double MaximumValue() const override;
    double DefaultValue() const override;
};

extern InpaintBlackPoint* TheInpaintBlackPointParameter;

class InpaintPrecision : public MetaInt8
{
public:
    InpaintPrecision(MetaProcess*);

    IsoString Id() const override;
    double MinimumValue() const override;
    double MaximumValue() const override;
    double DefaultValue() const override;
};

extern InpaintPrecision* TheInpaintPrecisionParameter;

PCL_END_LOCAL

}	// namespace pcl

#endif	// __InpaintParameters_h
