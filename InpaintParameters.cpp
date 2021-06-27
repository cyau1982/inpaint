#include "InpaintParameters.h"

namespace pcl
{

InpaintBlackPoint* TheInpaintBlackPointParameter = nullptr;
InpaintPrecision* TheInpaintPrecisionParameter = nullptr;

InpaintBlackPoint::InpaintBlackPoint(MetaProcess* P) : MetaFloat(P)
{
    TheInpaintBlackPointParameter = this;
}

IsoString InpaintBlackPoint::Id() const
{
    return "blackPoint";
}

int InpaintBlackPoint::Precision() const
{
    return 3;
}

double InpaintBlackPoint::MinimumValue() const
{
    return 0.0;
}

double InpaintBlackPoint::MaximumValue() const
{
    return 1.0;
}

double InpaintBlackPoint::DefaultValue() const
{
    return 0.0;
}

InpaintPrecision::InpaintPrecision(MetaProcess* P) : MetaInt8(P)
{
    TheInpaintPrecisionParameter = this;
}

IsoString InpaintPrecision::Id() const
{
    return "precision";
}

double InpaintPrecision::MinimumValue() const
{
    return 4.0;
}

double InpaintPrecision::MaximumValue() const
{
    return 32.0;
}

double InpaintPrecision::DefaultValue() const
{
    return 16.0;
}

}	// namespace pcl
