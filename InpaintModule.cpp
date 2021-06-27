#define MODULE_VERSION_MAJOR     1
#define MODULE_VERSION_MINOR     0
#define MODULE_VERSION_REVISION  0
#define MODULE_VERSION_BUILD     0
#define MODULE_VERSION_LANGUAGE  eng

#define MODULE_RELEASE_YEAR      2021
#define MODULE_RELEASE_MONTH     4
#define MODULE_RELEASE_DAY       3

#include "InpaintModule.h"
#include "InpaintProcess.h"
#include "InpaintInterface.h"

namespace pcl
{

InpaintModule::InpaintModule()
{
}

const char* InpaintModule::Version() const
{
    return PCL_MODULE_VERSION(MODULE_VERSION_MAJOR,
                              MODULE_VERSION_MINOR,
                              MODULE_VERSION_REVISION,
                              MODULE_VERSION_BUILD,
                              MODULE_VERSION_LANGUAGE);
}

IsoString InpaintModule::Name() const
{
    return "Inpaint";
}

String InpaintModule::Description() const
{
    return "PixInsight Inpaint Module";
}

String InpaintModule::Company() const
{
    return "N/A";
}

String InpaintModule::Author() const
{
    return "Johnny Qiu";
}

String InpaintModule::Copyright() const
{
    return "Copyright (c) 2021 Johnny Qiu";
}

String InpaintModule::TradeMarks() const
{
    return "JQ";
}

String InpaintModule::OriginalFileName() const
{
#ifdef __PCL_LINUX
    return "inpaint-pxm.so";
#endif
#ifdef __PCL_FREEBSD
    return "inpaint-pxm.so";
#endif
#ifdef __PCL_MACOSX
    return "inpaint-pxm.dylib";
#endif
#ifdef __PCL_WINDOWS
    return "inpaint-pxm.dll";
#endif
}

void InpaintModule::GetReleaseDate(int& year, int& month, int& day) const
{
    year = MODULE_RELEASE_YEAR;
    month = MODULE_RELEASE_MONTH;
    day = MODULE_RELEASE_DAY;
}

}   // namespace pcl

PCL_MODULE_EXPORT int InstallPixInsightModule(int mode)
{
    new pcl::InpaintModule;

    if (mode == pcl::InstallMode::FullInstall) {
        new pcl::InpaintProcess;
        new pcl::InpaintInterface;
    }

    return 0;
}
