#include "TBInterface.h"

#include <tb_core.h>
#include <tb_system.h>
#include <tb_renderer.h>
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

#define VER_COL(r, g, b, a) (((a)<<24) + ((b)<<16) + ((g)<<8) + r)
#define VER_COL_OPACITY(a) (0x00ffffff + (((tb::uint32)a) << 24))

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

    ~TBImageLoaderImpl()
    {

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

class TBRendererImpl : public tb::TBRenderer
{
    tb::uint8 m_opacity = 255;
    tb::TBRect m_screen_rect;
    tb::TBRect m_clip_rect;
    int m_translation_x = 0;
    int m_translation_y = 0;

    float m_u = 0.0f, m_v = 0.0f, m_uu = 0.0f, m_vv = 0.0f;

    /** A batch which should be rendered. */
    class Batch
    {
    public:
        Batch() : vertex_count(0), bitmap(nullptr), fragment(nullptr), batch_id(0), is_flushing(false) {}
        void Flush(TBRendererImpl *batch_renderer)
        {
            using namespace tb;

            if (!vertex_count || is_flushing)
                return;

            // Prevent re-entrancy. Calling fragment->GetBitmap may end up calling TBBitmap::SetData
            // which will end up flushing any existing batch with that bitmap.
            is_flushing = true;

            if (fragment)
            {
                // Now it's time to ensure the bitmap data is up to date. A call to GetBitmap
                // with TB_VALIDATE_ALWAYS should guarantee that its data is validated.
                TBBitmap *frag_bitmap = fragment->GetBitmap(TB_VALIDATE_ALWAYS);
                ((void)frag_bitmap); // silence warning about unused variable
                assert(frag_bitmap == bitmap);
            }

            batch_renderer->RenderBatch(this);

            vertex_count = 0;

            batch_id++; // Will overflow eventually, but that doesn't really matter.

            is_flushing = false;
        }

        Vertex *Reserve(TBRendererImpl *batch_renderer, int count)
        {
            //assert(count < VERTEX_BATCH_SIZE);
            //if (vertex_count + count > VERTEX_BATCH_SIZE)
            //    Flush(batch_renderer);
            //Vertex *ret = &vertex[vertex_count];
            //vertex_count += count;
            //return ret;
            return nullptr;
        }

        //Vertex vertex[VERTEX_BATCH_SIZE];
        int vertex_count;

        tb::TBBitmap *bitmap;
        tb::TBBitmapFragment *fragment;

        tb::uint32 batch_id;
        bool is_flushing;
    };


public:
    void BeginPaint(int render_target_w, int render_target_h) override
    {
        m_screen_rect.Set(0, 0, render_target_w, render_target_h);
        m_clip_rect = m_screen_rect;
    }

    virtual void EndPaint()
    {
        FlushAllInternal();
    }

    void Translate(int dx, int dy) override
    {
        m_translation_x += dx;
        m_translation_y += dy;
    }

    void SetOpacity(float opacity) override
    {
        tb::int8 opacity8 = (tb::uint8)(opacity * 255);
        if (opacity8 == m_opacity)
            return;
        m_opacity = opacity8;
    }

    float GetOpacity() override
    {
        return m_opacity / 255.f;
    }

    tb::TBRect SetClipRect(const tb::TBRect &rect, bool add_to_current) override
    {
        tb::TBRect old_clip_rect = m_clip_rect;
        m_clip_rect = rect;
        m_clip_rect.x += m_translation_x;
        m_clip_rect.y += m_translation_y;

        if (add_to_current)
            m_clip_rect = m_clip_rect.Clip(old_clip_rect);

        FlushAllInternal();
        SetClipRect(m_clip_rect);

        old_clip_rect.x -= m_translation_x;
        old_clip_rect.y -= m_translation_y;
        return old_clip_rect;
    }

    tb::TBRect GetClipRect() override
    {
        tb::TBRect curr_clip_rect = m_clip_rect;
        curr_clip_rect.x -= m_translation_x;
        curr_clip_rect.y -= m_translation_y;
        return curr_clip_rect;
    }

    void DrawBitmap(const tb::TBRect &dst_rect, const tb::TBRect &src_rect, tb::TBBitmapFragment *bitmap_fragment) override
    {
        if (tb::TBBitmap *bitmap = bitmap_fragment->GetBitmap(tb::TB_VALIDATE_FIRST_TIME))
            AddQuadInternal(dst_rect.Offset(m_translation_x, m_translation_y),
                            src_rect.Offset(bitmap_fragment->m_rect.x, bitmap_fragment->m_rect.y),
                            VER_COL_OPACITY(m_opacity), bitmap, bitmap_fragment);
    }

    void DrawBitmap(const tb::TBRect &dst_rect, const tb::TBRect &src_rect, tb::TBBitmap *bitmap) override
    {
        AddQuadInternal(dst_rect.Offset(m_translation_x, m_translation_y), src_rect, VER_COL_OPACITY(m_opacity), bitmap, nullptr);
    }

    void DrawBitmapColored(const tb::TBRect &dst_rect, const tb::TBRect &src_rect, const tb::TBColor &color, tb::TBBitmapFragment *bitmap_fragment) override
    {
        if (tb::TBBitmap *bitmap = bitmap_fragment->GetBitmap(tb::TB_VALIDATE_FIRST_TIME))
        {
            tb::uint32 a = (color.a * m_opacity) / 255;
            AddQuadInternal(dst_rect.Offset(m_translation_x, m_translation_y),
                            src_rect.Offset(bitmap_fragment->m_rect.x, bitmap_fragment->m_rect.y),
                            VER_COL(color.r, color.g, color.b, a), bitmap, bitmap_fragment);
        }
    }

    void DrawBitmapColored(const tb::TBRect &dst_rect, const tb::TBRect &src_rect, const tb::TBColor &color, tb::TBBitmap *bitmap) override
    {
        tb::uint32 a = (color.a * m_opacity) / 255;
        AddQuadInternal(dst_rect.Offset(m_translation_x, m_translation_y),
                        src_rect, VER_COL(color.r, color.g, color.b, a), bitmap, nullptr);
    }

    void DrawBitmapTile(const tb::TBRect &dst_rect, tb::TBBitmap *bitmap) override
    {
        AddQuadInternal(dst_rect.Offset(m_translation_x, m_translation_y),
                        tb::TBRect(0, 0, dst_rect.w, dst_rect.h),
                        VER_COL_OPACITY(m_opacity), bitmap, nullptr);
    }

    void DrawRect(const tb::TBRect &dst_rect, const tb::TBColor &color) override
    {
        if (dst_rect.IsEmpty())
            return;
        // Top
        DrawRectFill(tb::TBRect(dst_rect.x, dst_rect.y, dst_rect.w, 1), color);
        // Bottom
        DrawRectFill(tb::TBRect(dst_rect.x, dst_rect.y + dst_rect.h - 1, dst_rect.w, 1), color);
        // Left
        DrawRectFill(tb::TBRect(dst_rect.x, dst_rect.y + 1, 1, dst_rect.h - 2), color);
        // Right
        DrawRectFill(tb::TBRect(dst_rect.x + dst_rect.w - 1, dst_rect.y + 1, 1, dst_rect.h - 2), color);
    }

    void DrawRectFill(const tb::TBRect &dst_rect, const tb::TBColor &color) override
    {
        if (dst_rect.IsEmpty())
            return;
        tb::uint32 a = (color.a * m_opacity) / 255;
        AddQuadInternal(dst_rect.Offset(m_translation_x, m_translation_y),
                        tb::TBRect(), VER_COL(color.r, color.g, color.b, a), nullptr, nullptr);
    }

    void FlushBitmap(tb::TBBitmap *bitmap)
    {
        // Flush the batch if it's using this bitmap (that is about to change or be deleted)
        //if (batch.vertex_count && bitmap == batch.bitmap)
            //batch.Flush(this);
    }

    void FlushBitmapFragment(tb::TBBitmapFragment *bitmap_fragment) override
    {
        // Flush the batch if it is using this fragment (that is about to change or be deleted)
        // We know if it is in use in the current batch if its batch_id matches the current
        // batch_id in our (one and only) batch.
        // If we switch to a more advance batching system with multiple batches, we need to
        // solve this a bit differently.
        //if (batch.vertex_count && bitmap_fragment->m_batch_id == batch.batch_id)
        //    batch.Flush(this);
    }

    void AddQuadInternal(const tb::TBRect &dst_rect, const tb::TBRect &src_rect, tb::uint32 color, tb::TBBitmap *bitmap, tb::TBBitmapFragment *fragment)
    {

    }

    void FlushAllInternal()
    {
        //batch.Flush(this);
    }

    tb::TBBitmap* CreateBitmap(int width, int height, tb::uint32 *data) override
    {
        return nullptr;
    }

    void RenderBatch(Batch *batch)
    {

    }

    void SetClipRect(const tb::TBRect &rect)
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

    auto pixels = df3d::GetPixelBufferFromSource(file, 4);
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
