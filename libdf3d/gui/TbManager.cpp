#include "TbManager.h"

#include <third-party/turbobadger/src/tb/tb_core.h>
#include <third-party/turbobadger/src/tb/tb_system.h>
#include <third-party/turbobadger/src/tb/renderers/tb_renderer_batcher.h>
#include <third-party/turbobadger/src/tb/tb_bitmap_fragment.h>

namespace tb
{

TBImageLoader *TBImageLoader::CreateFromFile(const char *filename)
{
    return nullptr;
}

void TBSystem::RescheduleTimer(double fire_time)
{
}


}


namespace df3d {

namespace gui_impl {

class MyImageLoader : public tb::TBImageLoader
{
public:
    /*
    virtual int Width() = 0;
    virtual int Height() = 0;
    virtual uint32 *Data() = 0;
    */
};

class MyRenderer : public tb::TBRendererBatcher
{
public:
    virtual void BeginPaint(int render_target_w, int render_target_h)
    {

    }

    virtual void EndPaint()
    {

    }

    virtual tb::TBBitmap *CreateBitmap(int width, int height, tb::uint32 *data)
    {
        return nullptr;
    }

    // == TBRendererBatcher ===============================================================

    virtual void RenderBatch(Batch *batch)
    {

    }

    virtual void SetClipRect(const tb::TBRect &rect)
    {

    }
};

}


TBManager::TBManager()
{
    tb::TBRenderer *r = new gui_impl::MyRenderer();

    tb::tb_core_init(r);
}

TBManager::~TBManager()
{
    tb::tb_core_shutdown();
}

}
