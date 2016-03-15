#include "TBInterface.h"

#include <tb_core.h>
#include <tb_system.h>
#include <renderers/tb_renderer_batcher.h>
#include <tb_bitmap_fragment.h>
#include <Windows.h>

#include <libdf3d/base/EngineController.h>
#include <libdf3d/render/Texture.h>
#include <libdf3d/render/RenderManager.h>
#include <libdf3d/render/IRenderBackend.h>
#include <libdf3d/io/FileSystem.h>
#include <libdf3d/io/FileDataSource.h>
#include <libdf3d/resource_loaders/TextureLoaders.h>

namespace df3d { namespace gui_impl {

class TBFileImpl : public tb::TBFile
{
    shared_ptr<FileDataSource> m_file;

public:
    TBFileImpl(shared_ptr<FileDataSource> file)
        : m_file(file)
    {

    }

    ~TBFileImpl()
    {

    }

    long Size() override
    {
        return m_file->getSizeInBytes();
    }

    size_t Read(void *buf, size_t elemSize, size_t count) override
    {
        if (elemSize != 1)
        {
            DF3D_ASSERT(false, "not implemented");
            return 0;
        }

        return m_file->getRaw(buf, count);
    }
};

class TBImageLoaderImpl : public tb::TBImageLoader
{
    unique_ptr<unsigned char[]> m_data;
    int m_width, m_height;

public:
    TBImageLoaderImpl(const PixelBuffer &buffer)
        : m_width(buffer.getWidth()),
        m_height(buffer.getHeight())
    {
        m_data.reset(new unsigned char[buffer.getSizeInBytes()]);
        memcpy(m_data.get(), buffer.getData(), buffer.getSizeInBytes());
    }

    int Width() override
    {
        return m_width;
    }

    int Height() override
    {
        return m_height;
    }

    tb::uint32* Data() override
    {
        return (tb::uint32*)m_data.get();
    }
};

class TBRendererImpl : public tb::TBRendererBatcher
{
public:
    void BeginPaint(int render_target_w, int render_target_h) override
    {
        TBRendererBatcher::BeginPaint(render_target_w, render_target_h);
        // TODO_tb
    }

    virtual void EndPaint()
    {
        TBRendererBatcher::EndPaint();
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
        // TODO_tb
        /*

        glScissor(x, y, width, height);

        glScissor(m_clip_rect.x, m_screen_rect.h - (m_clip_rect.y + m_clip_rect.h), m_clip_rect.w, m_clip_rect.h);
        */
    }
};

tb::TBRenderer* CreateRenderer()
{
    return new TBRendererImpl();
}

} }

namespace tb
{

TBImageLoader* TBImageLoader::CreateFromFile(const char *filename)
{
    auto file = df3d::svc().fileSystem().openFile(filename);
    if (!file)
        return nullptr;

    auto pixels = df3d::GetPixelBufferFromSource(file);
    if (!pixels)
        return nullptr;

    if (pixels->getFormat() != df3d::PixelFormat::RGBA)
    {
        df3d::glog << "Unsupported tb image format" << df3d::logwarn;
        return nullptr;
    }

    return new df3d::gui_impl::TBImageLoaderImpl(*pixels);
}

void TBSystem::RescheduleTimer(double fire_time)
{

}

TBFile* TBFile::Open(const char *filename, TBFileMode mode)
{
    if (mode != MODE_READ)
        return nullptr;

    auto file = df3d::svc().fileSystem().openFile(filename);
    if (!file)
        return nullptr;

    return new df3d::gui_impl::TBFileImpl(file);
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
