#include "RocketInterface.h"

#include <base/EngineController.h>
#include <resources/ResourceManager.h>
#include <resources/ResourceFactory.h>
#include <io/FileSystem.h>
#include <io/FileDataSource.h>
#include <render/RenderManager.h>
#include <render/GpuProgram.h>
#include <render/Texture.h>
#include <render/RenderOperation.h>
#include <render/VertexIndexBuffer.h>
#include <render/RenderPass.h>
#include <render/RendererBackend.h>
#include <render/Texture2D.h>
#include <render/Viewport.h>
#include <render/RenderCommon.h>

#include <Rocket/Core.h>

namespace df3d { namespace gui_impl {

FileInterface::FileInterface()
{
}

FileInterface::~FileInterface()
{
}

Rocket::Core::FileHandle FileInterface::Open(const Rocket::Core::String& path)
{
    auto file = svc().fileSystem().openFile(path.CString());
    if (!file)
        return 0;

    auto handle = uintptr_t(file.get());

    m_openedFiles[handle] = file;
    return handle;
}

void FileInterface::Close(Rocket::Core::FileHandle file)
{
    auto erased = m_openedFiles.erase(file);
    if (erased != 1)
        glog << "Rocket interface couldn't erase unused file" << logwarn;
}

size_t FileInterface::Read(void* buffer, size_t size, Rocket::Core::FileHandle file)
{
    return m_openedFiles[file]->getRaw(buffer, size);
}

bool FileInterface::Seek(Rocket::Core::FileHandle file, long offset, int origin)
{
    std::ios_base::seekdir orig;
    if (origin == SEEK_CUR)
        orig = std::ios_base::cur;
    else if (origin == SEEK_SET)
        orig = std::ios_base::beg;
    else if (origin == SEEK_END)
        orig = std::ios_base::end;
    else
        return false;

    return m_openedFiles[file]->seek(offset, orig);
}

size_t FileInterface::Tell(Rocket::Core::FileHandle file)
{
    return static_cast<size_t>(m_openedFiles[file]->tell());
}

SystemInterface::SystemInterface()
{
    m_appStarted = std::chrono::system_clock::now();
}

float SystemInterface::GetElapsedTime()
{
    auto now = std::chrono::system_clock::now();
    auto dt = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_appStarted).count();

    return dt / 1000.f;
}

bool SystemInterface::LogMessage(Rocket::Core::Log::Type type, const Rocket::Core::String &message)
{
    switch (type)
    {
    case Rocket::Core::Log::LT_ERROR:
    case Rocket::Core::Log::LT_ASSERT:
        glog << message.CString() << logcritical;
        break;
    case Rocket::Core::Log::LT_WARNING:
        glog << message.CString() << logwarn;
        break;
    case Rocket::Core::Log::LT_ALWAYS:
    case Rocket::Core::Log::LT_INFO:
    case Rocket::Core::Log::LT_DEBUG:
        glog << message.CString() << logmess;
        break;
    default:
        break;
    }

    return true;
}

// Helper.
struct CompiledGeometry
{
    shared_ptr<Texture> texture;
    shared_ptr<VertexBuffer> vb;
    shared_ptr<IndexBuffer> ib;
};

RenderInterface::RenderInterface()
    : m_textureId(0)
{

}

void RenderInterface::SetViewport(int width, int height)
{

}

void RenderInterface::RenderGeometry(Rocket::Core::Vertex *vertices, int num_vertices,
                                        int *indices, int num_indices,
                                        Rocket::Core::TextureHandle texture,
                                        const Rocket::Core::Vector2f &translation)
{
    auto compiledGeometry = CompileGeometry(vertices, num_vertices, indices, num_indices, texture);
    RenderCompiledGeometry(compiledGeometry, translation);
    ReleaseCompiledGeometry(compiledGeometry);
}

Rocket::Core::CompiledGeometryHandle RenderInterface::CompileGeometry(Rocket::Core::Vertex *vertices, int num_vertices,
                                                                         int *indices, int num_indices,
                                                                         Rocket::Core::TextureHandle texture)
{
    VertexFormat vertexFormat({ VertexFormat::POSITION_3, VertexFormat::TX_2, VertexFormat::COLOR_4 });

    // FIXME: can map directly to vertexBuffer if Rocket::Core::Vertex is the same layout as internal vertex buffer storage.
    VertexData vertexData(vertexFormat);
    for (int i = 0; i < num_vertices; i++)
    {
        auto v = vertexData.allocVertex();

        v.setPosition({ vertices[i].position.x, vertices[i].position.y, 0.0f });
        v.setTx({ vertices[i].tex_coord.x, vertices[i].tex_coord.y });
        v.setColor({ vertices[i].colour.red / 255.0f, vertices[i].colour.green / 255.0f, vertices[i].colour.blue / 255.0f, vertices[i].colour.alpha / 255.0f });
    }

    // Set up vertex buffer.
    auto vertexBuffer = make_shared<VertexBuffer>(vertexFormat);
    vertexBuffer->alloc(vertexData, GpuBufferUsageType::STATIC);

    // FIXME: this is not 64 bit
    static_assert(sizeof(INDICES_TYPE) == sizeof(int), "rocket indices should be the same as render::INDICES_TYPE");

    // Set up index buffer
    auto indexBuffer = make_shared<IndexBuffer>();
    indexBuffer->alloc(num_indices, indices, GpuBufferUsageType::STATIC);

    CompiledGeometry *geom = new CompiledGeometry();
    geom->texture = m_textures[texture];        // NOTE: can be nullptr. It's okay.
    geom->vb = vertexBuffer;
    geom->ib = indexBuffer;

    return (Rocket::Core::CompiledGeometryHandle)geom;
}

void RenderInterface::RenderCompiledGeometry(Rocket::Core::CompiledGeometryHandle geometry,
                                                const Rocket::Core::Vector2f &translation)
{
    if (!geometry)
        return;

    if (!m_guipass)
    {
        m_guipass = make_shared<RenderPass>();

        m_guipass->setFaceCullMode(RenderPass::FaceCullMode::NONE);
        m_guipass->setGpuProgram(svc().resourceManager().getFactory().createColoredGpuProgram());
        m_guipass->enableDepthTest(false);
        m_guipass->enableDepthWrite(false);
        m_guipass->setBlendMode(RenderPass::BlendingMode::ALPHA);
    }

    auto geom = (CompiledGeometry *)geometry;

    m_guipass->setSampler("diffuseMap", geom->texture);

    RenderOperation op;
    op.vertexData = geom->vb;
    op.indexData = geom->ib;
    op.passProps = m_guipass;
    op.worldTransform = glm::translate(glm::vec3(translation.x, translation.y, 0.0f));

    // TODO_ecs: use render queue here!
    assert(false);
    //svc().renderManager().drawOperation(op);
}

void RenderInterface::ReleaseCompiledGeometry(Rocket::Core::CompiledGeometryHandle geometry)
{
    auto geom = (CompiledGeometry *)geometry;
    delete geom;
}

void RenderInterface::EnableScissorRegion(bool enable)
{
    svc().renderManager().getRenderer()->enableScissorTest(enable);
}

void RenderInterface::SetScissorRegion(int x, int y, int width, int height)
{
    svc().renderManager().getRenderer()->setScissorRegion(x, svc().getViewport().height() - (y + height), width, height);
}

bool RenderInterface::LoadTexture(Rocket::Core::TextureHandle &texture_handle,
                                     Rocket::Core::Vector2i &texture_dimensions,
                                     const Rocket::Core::String &source)
{
    TextureCreationParams params;
    params.setFiltering(TextureFiltering::BILINEAR);
    params.setMipmapped(false);
    params.setAnisotropyLevel(NO_ANISOTROPY);

    auto texture = svc().resourceManager().getFactory().createTexture(source.CString(), params, ResourceLoadingMode::IMMEDIATE);
    if (!texture || !texture->isInitialized())
        return false;

    m_textures[++m_textureId] = texture;
    texture_handle = m_textureId;

    texture_dimensions.x = texture->getOriginalWidth();
    texture_dimensions.y = texture->getOriginalHeight();

    return true;
}

bool RenderInterface::GenerateTexture(Rocket::Core::TextureHandle& texture_handle,
                                         const Rocket::Core::byte *source,
                                         const Rocket::Core::Vector2i &source_dimensions)
{
    TextureCreationParams params;
    params.setMipmapped(false);
    params.setAnisotropyLevel(NO_ANISOTROPY);
    params.setFiltering(TextureFiltering::BILINEAR);

    auto pb = make_unique<PixelBuffer>(source_dimensions.x, source_dimensions.y, source, PixelFormat::RGBA);

    auto texture = svc().resourceManager().getFactory().createTexture(std::move(pb), params);

    m_textures[++m_textureId] = texture;
    texture_handle = m_textureId;

    return true;
}

void RenderInterface::ReleaseTexture(Rocket::Core::TextureHandle texture_handle)
{
    auto guid = m_textures[texture_handle]->getGUID();

    // Erase from internal buffer.
    m_textures.erase(texture_handle);
    // Unload from resource manager.
    svc().resourceManager().unloadResource(guid);
}

} }
