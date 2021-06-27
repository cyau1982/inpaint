#include <pcl/Console.h>
#include <pcl/Arguments.h>
#include <pcl/View.h>
#include <pcl/Exception.h>

#include "InpaintProcess.h"
#include "InpaintParameters.h"
#include "InpaintInstance.h"
#include "InpaintInterface.h"

namespace pcl
{

InpaintProcess* TheInpaintProcess = nullptr;

InpaintProcess::InpaintProcess()
{
    TheInpaintProcess = this;

    // Instantiate process parameters
    new InpaintBlackPoint(this);
    new InpaintPrecision(this);
}

IsoString InpaintProcess::Id() const
{
    return "Inpaint";
}

IsoString InpaintProcess::Category() const
{
    return "Painting";
}

// ----------------------------------------------------------------------------

uint32 InpaintProcess::Version() const
{
    return 0x100;
}

// ----------------------------------------------------------------------------

String InpaintProcess::Description() const
{
    return "";
}

// ----------------------------------------------------------------------------

IsoString InpaintProcess::IconImageSVG() const
{
    return "<svg width=\"512\" height=\"512\" xmlns=\"http://www.w3.org/2000/svg\">"
           "<g id = \"Layer_1\">"
           "<title>Inpaint</title>"
           "<text transform = \"matrix(1.191 0 0 1.36283 -52.7099 -98.671)\" stroke=\"#000\" font-style=\"normal\" font-weight=\"normal\" xml:space=\"preserve\" text-anchor=\"start\" font-family=\"'Open Sans'\" font-size=\"120\" id=\"svg_1\" y=\"293.53247\" x=\"63.48111\" stroke-width=\"0\" fill=\"#ff0000\">Inpaint</text>"
           "</g>"
           "</svg>";
}
// ----------------------------------------------------------------------------

ProcessInterface* InpaintProcess::DefaultInterface() const
{
    return TheInpaintInterface;
}
// ----------------------------------------------------------------------------

ProcessImplementation* InpaintProcess::Create() const
{
    return new InpaintInstance(this);
}

// ----------------------------------------------------------------------------

ProcessImplementation* InpaintProcess::Clone(const ProcessImplementation& p) const
{
    const InpaintInstance* instPtr = dynamic_cast<const InpaintInstance*>(&p);
    return (instPtr != 0) ? new InpaintInstance(*instPtr) : 0;
}

// ----------------------------------------------------------------------------

bool InpaintProcess::NeedsValidation() const
{
    return false;
}

// ----------------------------------------------------------------------------

bool InpaintProcess::CanProcessCommandLines() const
{
    return false;
}

}	// namespace pcl
