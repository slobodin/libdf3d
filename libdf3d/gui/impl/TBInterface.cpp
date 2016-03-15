#include "TBInterface.h"

#include <tb_core.h>
#include <tb_system.h>
#include <renderers/tb_renderer_batcher.h>
#include <tb_bitmap_fragment.h>

namespace df3d { namespace gui_impl {

class TBImageLoaderImpl : public tb::TBImageLoader
{
public:
    int Width() override
    {
        return 0;
    }

    int Height() override
    {
        return 0;
    }

    tb::uint32* Data() override
    {
        return nullptr;
    }
};

class TBRendererImpl : public tb::TBRendererBatcher
{
public:
    void BeginPaint(int render_target_w, int render_target_h) override
    {

    }

    virtual void EndPaint()
    {

    }

    tb::TBBitmap* CreateBitmap(int width, int height, tb::uint32 *data) override
    {
        return nullptr;
    }

    void RenderBatch(Batch *batch) override
    {

    }

    void SetClipRect(const tb::TBRect &rect) override
    {

    }
};

tb::TBRenderer* CreateRenderer()
{
    return new TBRendererImpl();
}

} }

namespace tb
{

TBImageLoader *TBImageLoader::CreateFromFile(const char *filename)
{
    return new df3d::gui_impl::TBImageLoaderImpl();
}

void TBSystem::RescheduleTimer(double fire_time)
{
}

}

