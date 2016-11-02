#include "TBInterface.h"

#include <tb_core.h>
#include <tb_system.h>
#include <tb_renderer.h>
#include <tb_bitmap_fragment.h>

#include <df3d/engine/EngineController.h>
#include <df3d/engine/render/RenderManager.h>
#include <df3d/engine/render/IRenderBackend.h>
#include <df3d/engine/render/Material.h>
#include <df3d/engine/render/RenderOperation.h>
#include <df3d/engine/resources/ResourceManager.h>
#include <df3d/engine/resources/ResourceDataSource.h>
#include <df3d/engine/resources/ResourceFileSystem.h>
#include <df3d/engine/resources/TextureResource.h>
#include <df3d/lib/os/PlatformUtils.h>

namespace df3d {
namespace gui_impl {

class TBFileImpl : public tb::TBFile
{
    ResourceDataSource *m_dataSource;

public:
    TBFileImpl(ResourceDataSource *dataSource)
        : m_dataSource(dataSource)
    {
        DF3D_ASSERT(m_dataSource != nullptr);
    }

    ~TBFileImpl()
    {
        svc().resourceManager().getFS().close(m_dataSource);
    }

    long Size() override
    {
        return m_dataSource->getSize();
    }

    size_t Read(void *buf, size_t elemSize, size_t count) override
    {
        if (elemSize != 1)
        {
            DF3D_FATAL("not implemented");
            return 0;
        }

        return m_dataSource->read(buf, count);
    }
};

class TBImageLoaderImpl : public tb::TBImageLoader
{
    TextureResourceData *m_data = nullptr;

public:
    TBImageLoaderImpl(ResourceDataSource &dataSource)
    {
        m_data = LoadTexture_Workaround(dataSource, MemoryManager::allocDefault());
        DF3D_ASSERT(m_data);
        DF3D_ASSERT(m_data->info.format == PixelFormat::RGBA);
    }

    ~TBImageLoaderImpl()
    {
        MemoryManager::allocDefault().makeDelete(m_data);
    }

    int Width() override
    {
        return m_data->info.width;
    }

    int Height() override
    {
        return m_data->info.height;
    }

    tb::uint32* Data() override
    {
        return (tb::uint32*)m_data->pixels.data();
    }
};

// NOTE: this is crap.

class TBRendererImpl : public tb::TBRenderer
{
    tb::uint8 m_opacity = 255;
    tb::TBRect m_screen_rect;
    tb::TBRect m_clip_rect;
    int m_translation_x = 0;
    int m_translation_y = 0;

    float m_u = 0.0f, m_v = 0.0f, m_uu = 0.0f, m_vv = 0.0f;

    class TBBitmapImpl : public tb::TBBitmap
    {
    public:
        TBBitmapImpl(TBRendererImpl *renderer)
            : m_renderer(renderer)
        {

        }

        ~TBBitmapImpl()
        {
            m_renderer->FlushBitmap(this);
            svc().renderManager().getBackend().destroyTexture(m_texture);
        }

        bool Init(int width, int height, tb::uint32 *data)
        {
            DF3D_ASSERT(width == tb::TBGetNearestPowerOfTwo(width));
            DF3D_ASSERT(height == tb::TBGetNearestPowerOfTwo(height));

            m_w = width;
            m_h = height;

            TextureInfo info;
            info.width = width;
            info.height = height;
            info.numMips = 0;
            info.format = PixelFormat::RGBA;
            uint32_t flags = TEXTURE_FILTERING_BILINEAR | TEXTURE_WRAP_MODE_REPEAT;

            m_texture = svc().renderManager().getBackend().createTexture2D(info, flags, data, width * height * 4);

            return m_texture.isValid();
        }

        int Width() override { return m_w; }
        int Height() override { return m_h; }

        void SetData(tb::uint32 *data) override
        {
            svc().renderManager().getBackend().updateTexture(m_texture, m_w, m_h, data);
        }

    public:
        TBRendererImpl *m_renderer;
        int m_w = 0, m_h = 0;
        TextureHandle m_texture;
    };

    /** A batch which should be rendered. */
    struct Batch
    {
        static const uint16_t VERTEX_BATCH_SIZE = 1 * 2048;       // NOTE: using 16-bit indices.

        Vertex_p_tx_c vertexData[VERTEX_BATCH_SIZE];

        uint16_t vertex_count = 0;
        int quadsCount = 0;

        tb::TBBitmap *bitmap = nullptr;
        tb::TBBitmapFragment *fragment = nullptr;

        tb::uint32 batch_id = 0;
        bool is_flushing = false;

        Batch() = default;
        ~Batch() = default;

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
                DF3D_ASSERT(frag_bitmap == bitmap);
            }

            auto &backend = svc().renderManager().getBackend();
            auto vb = backend.createVertexBuffer(Vertex_p_tx_c::getFormat(), vertex_count, vertexData, GpuBufferUsageType::STREAM);

            batch_renderer->RenderBatch(this, vb);

            svc().renderManager().getBackend().destroyVertexBuffer(vb);

            vertex_count = 0;
            quadsCount = 0;

            batch_id++; // Will overflow eventually, but that doesn't really matter.

            is_flushing = false;
        }

        Vertex_p_tx_c* ReserveQuad(TBRendererImpl *batch_renderer)
        {
            if (vertex_count + 4 > VERTEX_BATCH_SIZE)
                Flush(batch_renderer);

            Vertex_p_tx_c *ret = &vertexData[vertex_count];
            vertex_count += 4;
            quadsCount++;

            return ret;
        }
    };

    unique_ptr<Batch> m_batch;
    RenderPass m_guipass;

    void InvokeContextLost() override
    {
        m_guipass = {};
        m_batch.reset();
        tb::TBRenderer::InvokeContextLost();
    }

    void InvokeContextRestored() override
    {
        m_batch = make_unique<Batch>();
        createGuiPass();
        tb::TBRenderer::InvokeContextRestored();
    }

    void createGuiPass()
    {
        m_guipass.faceCullMode = FaceCullMode::BACK;
        m_guipass.depthTest = false;
        m_guipass.depthWrite = false;
        m_guipass.blendMode = BlendingMode::ALPHA;
        m_guipass.setParam("material_diffuse", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

        m_guipass.program = svc().renderManager().getEmbedResources().coloredProgram;
        DF3D_ASSERT(m_guipass.program != nullptr);
    }

    IndexBufferHandle m_indexBuffer;

public:
    TBRendererImpl()
    {
        m_batch = make_unique<Batch>();
        createGuiPass();

        // Initialize the index array.
        PodArray<uint16_t> indexData(MemoryManager::allocDefault());
        indexData.resize(Batch::VERTEX_BATCH_SIZE * 6);

        uint16_t currentIndex = 0;
        for (uint16_t i = 0; i < Batch::VERTEX_BATCH_SIZE; ++i)
        {
            // 4 vertices per quad
            indexData[currentIndex++] = 4 * i + 0;
            indexData[currentIndex++] = 4 * i + 1;
            indexData[currentIndex++] = 4 * i + 2;
            indexData[currentIndex++] = 4 * i + 1;
            indexData[currentIndex++] = 4 * i + 3;
            indexData[currentIndex++] = 4 * i + 2;
        }
        m_indexBuffer = svc().renderManager().getBackend().createIndexBuffer(indexData.size(), indexData.data(), GpuBufferUsageType::STATIC, INDICES_16_BIT);
    }

    ~TBRendererImpl()
    {
        svc().renderManager().getBackend().destroyIndexBuffer(m_indexBuffer);
    }

    void BeginPaint(int render_target_w, int render_target_h) override
    {
        m_screen_rect.Set(0, 0, render_target_w, render_target_h);
        m_clip_rect = m_screen_rect;

        svc().renderManager().getBackend().enableScissorTest(true);
        svc().renderManager().getBackend().setScissorRegion(0, 0, render_target_w, render_target_h);
    }

    virtual void EndPaint() override
    {
        FlushAllInternal();
        svc().renderManager().getBackend().enableScissorTest(false);
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

        svc().renderManager().getBackend().setScissorRegion(m_clip_rect.x, m_screen_rect.h - (m_clip_rect.y + m_clip_rect.h), m_clip_rect.w, m_clip_rect.h);

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
                            tb::TBColor(255, 255, 255, m_opacity), bitmap, bitmap_fragment);
    }

    void DrawBitmap(const tb::TBRect &dst_rect, const tb::TBRect &src_rect, tb::TBBitmap *bitmap) override
    {
        AddQuadInternal(dst_rect.Offset(m_translation_x, m_translation_y), src_rect, tb::TBColor(255, 255, 255, m_opacity), bitmap, nullptr);
    }

    void DrawBitmapColored(const tb::TBRect &dst_rect, const tb::TBRect &src_rect, const tb::TBColor &color, tb::TBBitmapFragment *bitmap_fragment) override
    {
        if (tb::TBBitmap *bitmap = bitmap_fragment->GetBitmap(tb::TB_VALIDATE_FIRST_TIME))
        {
            tb::uint32 a = (color.a * m_opacity) / 255;
            AddQuadInternal(dst_rect.Offset(m_translation_x, m_translation_y),
                            src_rect.Offset(bitmap_fragment->m_rect.x, bitmap_fragment->m_rect.y),
                            tb::TBColor(color.r, color.g, color.b, a), bitmap, bitmap_fragment);
        }
    }

    void DrawBitmapColored(const tb::TBRect &dst_rect, const tb::TBRect &src_rect, const tb::TBColor &color, tb::TBBitmap *bitmap) override
    {
        tb::uint32 a = (color.a * m_opacity) / 255;
        AddQuadInternal(dst_rect.Offset(m_translation_x, m_translation_y),
                        src_rect, tb::TBColor(color.r, color.g, color.b, a), bitmap, nullptr);
    }

    void DrawBitmapTile(const tb::TBRect &dst_rect, tb::TBBitmap *bitmap) override
    {
        AddQuadInternal(dst_rect.Offset(m_translation_x, m_translation_y),
                        tb::TBRect(0, 0, dst_rect.w, dst_rect.h),
                        tb::TBColor(255, 255, 255, m_opacity), bitmap, nullptr);
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
                        tb::TBRect(), tb::TBColor(color.r, color.g, color.b, a), nullptr, nullptr);
    }

    void FlushBitmap(tb::TBBitmap *bitmap)
    {
        // Flush the batch if it's using this bitmap (that is about to change or be deleted)
        if (m_batch && m_batch->vertex_count && bitmap == m_batch->bitmap)
            m_batch->Flush(this);
    }

    void FlushBitmapFragment(tb::TBBitmapFragment *bitmap_fragment) override
    {
        // Flush the batch if it is using this fragment (that is about to change or be deleted)
        // We know if it is in use in the current batch if its batch_id matches the current
        // batch_id in our (one and only) batch.
        // If we switch to a more advance batching system with multiple batches, we need to
        // solve this a bit differently.
        if (m_batch && m_batch->vertex_count && bitmap_fragment->m_batch_id == m_batch->batch_id)
            m_batch->Flush(this);
    }

    void AddQuadInternal(const tb::TBRect &dst_rect, const tb::TBRect &src_rect, const tb::TBColor &color, tb::TBBitmap *bitmap, tb::TBBitmapFragment *fragment)
    {
        if (UNLIKELY(!m_batch))
            return;

        if (m_batch->bitmap != bitmap)
        {
            m_batch->Flush(this);
            m_batch->bitmap = bitmap;
        }
        m_batch->fragment = fragment;
        if (bitmap)
        {
            int bitmap_w = bitmap->Width();
            int bitmap_h = bitmap->Height();
            m_u = (float)src_rect.x / bitmap_w;
            m_v = (float)src_rect.y / bitmap_h;
            m_uu = (float)(src_rect.x + src_rect.w) / bitmap_w;
            m_vv = (float)(src_rect.y + src_rect.h) / bitmap_h;
        }

        glm::vec4 glmcolor = { color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, color.a / 255.0f };

        auto ver = m_batch->ReserveQuad(this);
        ver[0].pos.x = (float)dst_rect.x;
        ver[0].pos.y = (float)(dst_rect.y + dst_rect.h);
        ver[0].uv.x = m_u;
        ver[0].uv.y = m_vv;
        ver[0].color = glmcolor;
        ver[1].pos.x = (float)(dst_rect.x + dst_rect.w);
        ver[1].pos.y = (float)(dst_rect.y + dst_rect.h);
        ver[1].uv.x = m_uu;
        ver[1].uv.y = m_vv;
        ver[1].color = glmcolor;
        ver[2].pos.x = (float)dst_rect.x;
        ver[2].pos.y = (float)dst_rect.y;
        ver[2].uv.x = m_u;
        ver[2].uv.y = m_v;
        ver[2].color = glmcolor;
        ver[3].pos.x = (float)(dst_rect.x + dst_rect.w);
        ver[3].pos.y = (float)dst_rect.y;
        ver[3].uv.x = m_uu;
        ver[3].uv.y = m_v;
        ver[3].color = glmcolor;

        // Update fragments batch id (See FlushBitmapFragment)
        if (fragment)
            fragment->m_batch_id = m_batch->batch_id;
    }

    void FlushAllInternal()
    {
        if (m_batch)
            m_batch->Flush(this);
    }

    tb::TBBitmap* CreateBitmap(int width, int height, tb::uint32 *data) override
    {
        auto bitmap = new TBBitmapImpl(this);
        bitmap->Init(width, height, data);
        return bitmap;
    }

    void RenderBatch(Batch *batch, VertexBufferHandle vb)
    {
        TextureHandle texture;
        if (batch->bitmap)
            texture = static_cast<TBBitmapImpl*>(batch->bitmap)->m_texture;
        else
            texture = svc().renderManager().getEmbedResources().whiteTexture;

        m_guipass.setParam("diffuseMap", texture);

        RenderOperation op;
        op.vertexBuffer = vb;
        op.indexBuffer = m_indexBuffer;
        op.passProps = &m_guipass;
        op.numberOfElements = batch->quadsCount * 6;  // 6 indices per quad

        svc().renderManager().drawRenderOperation(op);
    }
};

unique_ptr<tb::TBRenderer> CreateRenderer()
{
    return make_unique<TBRendererImpl>();
}

}
}

namespace tb
{

TBImageLoader* TBImageLoader::CreateFromFile(const char *filename)
{
    auto dataSource = df3d::svc().resourceManager().getFS().open(filename);
    if (!dataSource)
        return nullptr;

    auto result = new df3d::gui_impl::TBImageLoaderImpl(*dataSource);

    df3d::svc().resourceManager().getFS().close(dataSource);

    return result;
}

void TBSystem::RescheduleTimer(double fire_time)
{

}

TBFile* TBFile::Open(const char *filename, TBFileMode mode)
{
    if (mode != MODE_READ)
        return nullptr;

    if (auto dataSource = df3d::svc().resourceManager().getFS().open(filename))
        return new df3d::gui_impl::TBFileImpl(dataSource);
    return nullptr;
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

    return static_cast<double>(duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count());
}

int TBSystem::GetLongClickDelayMS()
{
    return 500;
}

int TBSystem::GetPanThreshold()
{
#if defined(DF3D_DESKTOP)
    return 5 * GetDPI() / 96;
#elif defined(DF3D_ANDROID)
    return 5 * GetDPI() / 120;
#elif defined(DF3D_IOS)
    return 5 * GetDPI() / 120;
#else
#error "Unsupported"
#endif
}

int TBSystem::GetPixelsPerLine()
{
#if defined(DF3D_DESKTOP)
    return 40 * GetDPI() / 96;
#elif defined(DF3D_ANDROID)
    return 40 * GetDPI() / 120;
#elif defined(DF3D_IOS)
    return 40 * GetDPI() / 120;
#else
#error "Unsupported"
#endif
}

int TBSystem::GetDPI()
{
    static int dpi = df3d::PlatformUtils::getDPI();
    return dpi;
}

}

#ifdef TB_RUNTIME_DEBUG_INFO
void TBDebugOut(const char *str)
{
    DFLOG_DEBUG(str);
}
#endif
