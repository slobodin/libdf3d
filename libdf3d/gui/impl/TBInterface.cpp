#include "TBInterface.h"

#include <tb_core.h>
#include <tb_system.h>
#include <renderers/tb_renderer_batcher.h>
#include <tb_bitmap_fragment.h>
#include <Windows.h>

namespace df3d { namespace gui_impl {

class TBFileImpl : public tb::TBFile
{
public:
    long Size() override
    {
        return 0;
    }

    size_t Read(void *buf, size_t elemSize, size_t count) override
    {
        return 0;
    }
};

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

TBFile* TBFile::Open(const char *filename, TBFileMode mode)
{
    return new df3d::gui_impl::TBFileImpl();
}

void TBClipboard::Empty()
{

}

bool TBClipboard::HasText()
{
    return false;
}

bool TBClipboard::SetText(const char *text)
{
    return false;
}

bool TBClipboard::GetText(TBStr &text)
{
    return false;
}

double TBSystem::GetTimeMS()
{
    using namespace std::chrono;

    return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
}

int TBSystem::GetLongClickDelayMS()
{
    return 500;
}

int TBSystem::GetPanThreshold()
{
    return 5 * GetDPI() / 96;
}

int TBSystem::GetPixelsPerLine()
{
    return 40 * GetDPI() / 96;
}

int TBSystem::GetDPI()
{
    HDC hdc = GetDC(nullptr);
    int DPI_x = GetDeviceCaps(hdc, LOGPIXELSX);
    ReleaseDC(nullptr, hdc);

    return DPI_x;
}

}

void TBDebugOut(const char *str)
{
    df3d::glog << str << df3d::logdebug;
}
