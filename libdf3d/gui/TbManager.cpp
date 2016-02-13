#include "TbManager.h"

#include <third-party/turbobadger/src/tb/tb_core.h>
#include <third-party/turbobadger/src/tb/tb_bitmap_fragment.h>

namespace tb
{

class MyImageLoader : public TBImageLoader
{
public:
    /*
    virtual int Width() = 0;
    virtual int Height() = 0;
    virtual uint32 *Data() = 0;
    */
};

TBImageLoader *TBImageLoader::CreateFromFile(const char *filename)
{
    return nullptr;
}

}


namespace df3d {

TBManager::TBManager()
{

}

TBManager::~TBManager()
{

}

}
