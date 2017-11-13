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
            DF3D_ASSERT_MESS(false, "not implemented");
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
        DF3D_ASSERT(m_data->mipLevels.size() == 1);
        DF3D_ASSERT(m_data->format == PixelFormat::RGBA);
    }

    ~TBImageLoaderImpl()
    {
        MAKE_DELETE(MemoryManager::allocDefault(), m_data);
    }

    int Width() override
    {
        if (!m_data)
            return 0;
        return m_data->mipLevels[0].width;
    }

    int Height() override
    {
        if (!m_data)
            return 0;
        return m_data->mipLevels[0].height;
    }

    tb::uint32* Data() override
    {
        if (!m_data)
            return nullptr;
        return (tb::uint32*)m_data->mipLevels[0].pixels.data();
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

            TextureResourceData resource;
            resource.format = PixelFormat::RGBA;

            TextureResourceData::MipLevel mipLvl;
            mipLvl.width = width;
            mipLvl.height = height;
            if (data != nullptr)
            {
                mipLvl.pixels.resize(4 * width * height);
                memcpy(mipLvl.pixels.data(), data, mipLvl.pixels.size());
            }
            resource.mipLevels.push_back(std::move(mipLvl));

            m_texture = svc().renderManager().getBackend().createTexture(resource, TEXTURE_FILTERING_BILINEAR | TEXTURE_WRAP_MODE_REPEAT);

            return m_texture.isValid();
        }

        int Width() override { return m_w; }
        int Height() override { return m_h; }

        void SetData(tb::uint32 *data) override
        {
            svc().renderManager().getBackend().updateTexture(m_texture, 0, 0, m_w, m_h, data);
        }

    public:
        TBRendererImpl *m_renderer;
        int m_w = 0, m_h = 0;
        TextureHandle m_texture;
    };

    class TBBatchRendering : NonCopyable
    {
        enum {
            // NOTE: using 16-bit indices.
            MAX_VERTICES = (1 << 14) - 512
        };

        struct Batch
        {
            tb::TBRect clipRect;
            TextureHandle texture;
            int startVertex = 0;
            int quadsCount = 0;
        };

        RenderPass m_guipass;
        std::vector<Batch> m_batches;

        Vertex_p_tx_c m_vertexData[MAX_VERTICES];

        int m_currentVerticesCount = 0;
        int m_currentBatchQuadsCount = 0;
        int m_currentBatchStartVertex = 0;
        tb::TBBitmap *m_currentBitmap = nullptr;
        tb::TBBitmapFragment *m_currentBitmapFragment = nullptr;
        tb::TBRect m_currentClipRect;

        int m_currBatchId = 0;

        VertexBufferHandle m_vbHandle;
        IndexBufferHandle m_ibHandle;

        bool m_isFlushing = false;

        float m_u = 0.0f, m_v = 0.0f, m_uu = 0.0f, m_vv = 0.0f;

        Vertex_p_tx_c* allocQuad()
        {
            if (m_currentVerticesCount + 4 >= MAX_VERTICES)
            {
                // FIXME: alloc one more GPU buffer.
                DF3D_ASSERT_MESS(false, "Vertices limit for GUI rendering.");
                return nullptr;
            }

            Vertex_p_tx_c *ret = &m_vertexData[m_currentVerticesCount];
            m_currentVerticesCount += 4;
            m_currentBatchQuadsCount++;

            return ret;
        }

        void flush()
        {
            if (m_currentBatchQuadsCount == 0 || m_isFlushing)
                return;

            m_isFlushing = true;

            if (m_currentBitmapFragment)
            {
                // Now it's time to ensure the bitmap data is up to date. A call to GetBitmap
                // with TB_VALIDATE_ALWAYS should guarantee that its data is validated.
                tb::TBBitmap *frag_bitmap = m_currentBitmapFragment->GetBitmap(tb::TB_VALIDATE_ALWAYS);
                ((void)frag_bitmap); // silence warning about unused variable
                DF3D_ASSERT(frag_bitmap == m_currentBitmap);
            }

            Batch batch;
            batch.clipRect = m_currentClipRect;
            batch.startVertex = m_currentBatchStartVertex;
            batch.quadsCount = m_currentBatchQuadsCount;
            if (m_currentBitmap)
                batch.texture = static_cast<TBBitmapImpl*>(m_currentBitmap)->m_texture;
            else
                batch.texture = svc().renderManager().getEmbedResources().whiteTexture;

            m_batches.push_back(batch);

            m_currentBatchQuadsCount = 0;
            m_currentBatchStartVertex = m_currentVerticesCount;
            m_currBatchId++; // Will overflow eventually, but that doesn't really matter.
            m_isFlushing = false;
        }

    public:
        TBBatchRendering()
        {
            auto &backend = svc().renderManager().getBackend();

            // Create vertex buffer.
            m_vbHandle = backend.createDynamicVertexBuffer(Vertex_p_tx_c::getFormat(),
                                                           MAX_VERTICES,
                                                           m_vertexData);

            // Setup GUI pass.
            m_guipass.setDepthTest(false);
            m_guipass.setDepthWrite(false);
            m_guipass.setBlending(Blending::ALPHA);
            m_guipass.setParam(Id("material_diffuse"), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

            m_guipass.program = svc().renderManager().getEmbedResources().coloredProgram;
            DF3D_ASSERT(m_guipass.program != nullptr);

            // Setup index buffer.
            PodArray<uint16_t> indexData(MemoryManager::allocDefault());
            indexData.resize(MAX_VERTICES * 6);

            int currentIndex = 0;
            for (uint16_t i = 0; i < MAX_VERTICES; ++i)
            {
                // 4 vertices per quad
                indexData[currentIndex++] = 4 * i + 0;
                indexData[currentIndex++] = 4 * i + 1;
                indexData[currentIndex++] = 4 * i + 2;
                indexData[currentIndex++] = 4 * i + 1;
                indexData[currentIndex++] = 4 * i + 3;
                indexData[currentIndex++] = 4 * i + 2;
            }

            m_ibHandle = backend.createIndexBuffer(indexData.size(), indexData.data(), INDICES_16_BIT);
        }

        ~TBBatchRendering()
        {
            auto &backend = svc().renderManager().getBackend();
            backend.destroyVertexBuffer(m_vbHandle);
            backend.destroyIndexBuffer(m_ibHandle);
        }

        void renderBatches()
        {
            flush();

            auto &backend = svc().renderManager().getBackend();

            if (m_currentVerticesCount > 0)
                backend.updateVertexBuffer(m_vbHandle, 0, m_currentVerticesCount, m_vertexData);

            for (const auto &batch : m_batches)
            {
                RenderOperation op;

                m_guipass.setParam(Id("diffuseMap"), batch.texture);

                op.vertexBuffer = m_vbHandle;
                op.indexBuffer = m_ibHandle;
                op.passProps = &m_guipass;
                op.numberOfElements = batch.quadsCount * 6;  // 6 indices per quad
                op.startVertex = batch.startVertex;

                auto backendID = backend.getID();
                Viewport scissorRect;

                if (backendID == RenderBackendID::METAL)
                {
                    const auto &r = batch.clipRect;
                    scissorRect.originX = r.x;
                    scissorRect.originY = r.y;
                    scissorRect.width = r.w;
                    scissorRect.height = r.h;
                }
                else
                {
                    const auto &r = batch.clipRect;
                    int h = svc().getScreenSize().y;
                    scissorRect.originX = r.x;
                    scissorRect.originY = h - (r.y + r.h);
                    scissorRect.width = r.w;
                    scissorRect.height = r.h;
                }

                backend.setScissorTest(true, scissorRect);

                svc().renderManager().drawRenderOperation(op);
            }

            m_currentVerticesCount = 0;
            m_currentBatchQuadsCount = 0;
            m_currentBatchStartVertex = 0;
            m_batches.clear();
            m_currentBitmap = nullptr;
            m_currentBitmapFragment = nullptr;
            m_isFlushing = false;
        }

        void addQuad(const tb::TBRect &dst_rect, const tb::TBRect &src_rect,
                     const tb::TBColor &color, tb::TBBitmap *bitmap, tb::TBBitmapFragment *fragment)
        {
            if (m_currentBitmap != bitmap)
            {
                flush();
                m_currentBitmap = bitmap;
            }
            m_currentBitmapFragment = fragment;

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

            auto ver = allocQuad();
            if (!ver)
                return;

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
                fragment->m_batch_id = m_currBatchId;
        }

        void tryFlush(tb::TBBitmap *bitmap)
        {
            // Flush the batch if it's using this bitmap (that is about to change or be deleted)
            if (bitmap == m_currentBitmap)
                flush();
        }

        void tryFlush(tb::TBBitmapFragment *bitmapFragment)
        {
            if (bitmapFragment->m_batch_id == m_currBatchId)
                flush();
        }

        void forceFlush()
        {
            flush();
        }

        void setClipRect(const tb::TBRect &r) { m_currentClipRect = r; }
    };

    void InvokeContextLost() override
    {
        m_batchRendering.reset();
        tb::TBRenderer::InvokeContextLost();
    }

    void InvokeContextRestored() override
    {
        m_batchRendering = make_unique<TBBatchRendering>();
        tb::TBRenderer::InvokeContextRestored();
    }

    unique_ptr<TBBatchRendering> m_batchRendering;

public:
    TBRendererImpl()
    {
        m_batchRendering = make_unique<TBBatchRendering>();
    }

    ~TBRendererImpl()
    {

    }

    void BeginPaint(int render_target_w, int render_target_h) override
    {
        m_screen_rect.Set(0, 0, render_target_w, render_target_h);
        m_clip_rect = m_screen_rect;
        if (m_batchRendering)
            m_batchRendering->setClipRect(m_clip_rect);

        svc().renderManager().getBackend().setScissorTest(true, { 0, 0, render_target_w, render_target_h });
    }

    virtual void EndPaint() override
    {
        if (m_batchRendering)
            m_batchRendering->renderBatches();

        svc().renderManager().getBackend().setScissorTest(false, {});
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

        if (m_batchRendering)
        {
            m_batchRendering->forceFlush();
            m_batchRendering->setClipRect(m_clip_rect);
        }

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
        {
            m_batchRendering->addQuad(dst_rect.Offset(m_translation_x, m_translation_y),
                                      src_rect.Offset(bitmap_fragment->m_rect.x, bitmap_fragment->m_rect.y),
                                      tb::TBColor(255, 255, 255, m_opacity), bitmap, bitmap_fragment);
        }
    }

    void DrawBitmap(const tb::TBRect &dst_rect, const tb::TBRect &src_rect, tb::TBBitmap *bitmap) override
    {
        m_batchRendering->addQuad(dst_rect.Offset(m_translation_x, m_translation_y),
                                  src_rect,
                                  tb::TBColor(255, 255, 255, m_opacity),
                                  bitmap, nullptr);
    }

    void DrawBitmapColored(const tb::TBRect &dst_rect, const tb::TBRect &src_rect, const tb::TBColor &color, tb::TBBitmapFragment *bitmap_fragment) override
    {
        if (tb::TBBitmap *bitmap = bitmap_fragment->GetBitmap(tb::TB_VALIDATE_FIRST_TIME))
        {
            tb::uint32 a = (color.a * m_opacity) / 255;
            m_batchRendering->addQuad(dst_rect.Offset(m_translation_x, m_translation_y),
                                      src_rect.Offset(bitmap_fragment->m_rect.x, bitmap_fragment->m_rect.y),
                                      tb::TBColor(color.r, color.g, color.b, a), bitmap, bitmap_fragment);
        }
    }

    void DrawBitmapColored(const tb::TBRect &dst_rect, const tb::TBRect &src_rect, const tb::TBColor &color, tb::TBBitmap *bitmap) override
    {
        tb::uint32 a = (color.a * m_opacity) / 255;
        m_batchRendering->addQuad(dst_rect.Offset(m_translation_x, m_translation_y),
                                  src_rect, tb::TBColor(color.r, color.g, color.b, a), bitmap, nullptr);
    }

    void DrawBitmapTile(const tb::TBRect &dst_rect, tb::TBBitmap *bitmap) override
    {
        m_batchRendering->addQuad(dst_rect.Offset(m_translation_x, m_translation_y),
                                  tb::TBRect(0, 0, dst_rect.w, dst_rect.h),
                                  tb::TBColor(255, 255, 255, m_opacity), bitmap, nullptr);
    }

    void FlushBitmap(tb::TBBitmap *bitmap)
    {
        // Flush the batch if it's using this bitmap (that is about to change or be deleted)
        if (m_batchRendering)
            m_batchRendering->tryFlush(bitmap);
    }

    void FlushBitmapFragment(tb::TBBitmapFragment *bitmap_fragment) override
    {
        // Flush the batch if it is using this fragment (that is about to change or be deleted)
        // We know if it is in use in the current batch if its batch_id matches the current
        // batch_id in our (one and only) batch.
        // If we switch to a more advance batching system with multiple batches, we need to
        // solve this a bit differently.
        if (m_batchRendering)
            m_batchRendering->tryFlush(bitmap_fragment);
    }

    tb::TBBitmap* CreateBitmap(int width, int height, tb::uint32 *data) override
    {
        auto bitmap = new TBBitmapImpl(this);
        bitmap->Init(width, height, data);
        return bitmap;
    }
};

unique_ptr<tb::TBRenderer> CreateTBRenderer()
{
    return make_unique<TBRendererImpl>();
}

}

namespace tb
{

TBImageLoader* TBImageLoader::CreateFromFile(const char *filename)
{
    auto dataSource = df3d::svc().resourceManager().getFS().open(filename);
    if (!dataSource)
        return nullptr;

    auto result = new df3d::TBImageLoaderImpl(*dataSource);

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
        return new df3d::TBFileImpl(dataSource);
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
