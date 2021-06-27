#ifndef __InpaintModule_h
#define __InpaintModule_h

#include <pcl/MetaModule.h>

namespace pcl
{

class InpaintModule : public MetaModule
{
public:
    InpaintModule();

    const char* Version() const override;
    IsoString Name() const override;
    String Description() const override;
    String Company() const override;
    String Author() const override;
    String Copyright() const override;
    String TradeMarks() const override;
    String OriginalFileName() const override;
    void GetReleaseDate(int& year, int& month, int& day) const override;
};

}   // namespace pcl

#endif  // __InpaintModule_h
