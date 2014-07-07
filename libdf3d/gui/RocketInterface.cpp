#include "df3d_pch.h"
#include "RocketInterface.h"

#include <base/Controller.h>
#include <resources/FileSystem.h>
#include <resources/FileDataSource.h>
#include <resources/ResourceManager.h>
#include <render/GpuProgram.h>
#include <render/Texture.h>
#include <render/RenderOperation.h>
#include <render/VertexIndexBuffer.h>
#include <render/RenderPass.h>
#include <render/Texture.h>
#include <render/RenderManager.h>
#include <render/Viewport.h>
#include <render/Image.h>

#include <Rocket/Core.h>

namespace df3d { namespace gui {

GuiFileInterface::GuiFileInterface()
{
}

GuiFileInterface::~GuiFileInterface()
{
}

Rocket::Core::FileHandle GuiFileInterface::Open(const Rocket::Core::String& path)
{
    auto file = g_fileSystem->openFile(path.CString());
    if (!file)
        return 0;

    auto handle = uintptr_t(file.get());

    m_openedFiles[handle] = file;
    return handle;
}

void GuiFileInterface::Close(Rocket::Core::FileHandle file)
{
    auto erased = m_openedFiles.erase(file);
    if (erased != 1)
        base::glog << "Rocket interface couldn't erase unused file" << base::logwarn;
}

size_t GuiFileInterface::Read(void* buffer, size_t size, Rocket::Core::FileHandle file)
{
    return m_openedFiles[file]->getRaw(buffer, size);
}

bool GuiFileInterface::Seek(Rocket::Core::FileHandle file, long offset, int origin)
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

size_t GuiFileInterface::Tell(Rocket::Core::FileHandle file)
{
    return m_openedFiles[file]->tell();
}

GuiSystemInterface::GuiSystemInterface()
{
    m_appStarted = std::chrono::system_clock::now();
}

float GuiSystemInterface::GetElapsedTime()
{
    auto now = std::chrono::system_clock::now();
    auto dt = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_appStarted).count();

    return dt / 1000.f;
}

bool GuiSystemInterface::LogMessage(Rocket::Core::Log::Type type, const Rocket::Core::String &message)
{
    switch (type)
    {
    case Rocket::Core::Log::LT_ERROR:
    case Rocket::Core::Log::LT_ASSERT:
        base::glog << message.CString() << base::logcritical;
        break;
    case Rocket::Core::Log::LT_WARNING:
        base::glog << message.CString() << base::logwarn;
        break;
    case Rocket::Core::Log::LT_ALWAYS:
    case Rocket::Core::Log::LT_INFO:
    case Rocket::Core::Log::LT_DEBUG:
        base::glog << message.CString() << base::logmess;
        break;
    default:
        break;
    }
    
    return true;
}

// Helper.
struct CompiledGeometry
{
    shared_ptr<render::Texture> texture;
    shared_ptr<render::VertexBuffer> vb;
    shared_ptr<render::IndexBuffer> ib;
};

GuiRenderInterface::GuiRenderInterface()
    : m_textureId(0),
    m_renderOperation(new render::RenderOperation())
{
    m_ffp2d = render::GpuProgram::createFFP2DGpuProgram();

    m_guipass = make_shared<render::RenderPass>();
    m_guipass->setFaceCullMode(render::RenderPass::FCM_NONE);
    m_guipass->setGpuProgram(m_ffp2d);
    m_guipass->enableDepthTest(false);
    m_guipass->setBlendMode(render::RenderPass::BM_ALPHA);
}

void GuiRenderInterface::SetViewport(int width, int height)
{

}

void GuiRenderInterface::RenderGeometry(Rocket::Core::Vertex *vertices, int num_vertices, 
                                        int *indices, int num_indices, 
                                        Rocket::Core::TextureHandle texture, 
                                        const Rocket::Core::Vector2f &translation)
{
    auto compiledGeometry = CompileGeometry(vertices, num_vertices, indices, num_indices, texture);
    RenderCompiledGeometry(compiledGeometry, translation);
    ReleaseCompiledGeometry(compiledGeometry);
}

Rocket::Core::CompiledGeometryHandle GuiRenderInterface::CompileGeometry(Rocket::Core::Vertex *vertices, int num_vertices, 
                                                                         int *indices, int num_indices, 
                                                                         Rocket::Core::TextureHandle texture)
{
    auto vertexBuffer = make_shared<render::VertexBuffer>(render::VertexFormat::create("p:2, tx:2, c:4"));
    auto indexBuffer = make_shared<render::IndexBuffer>();

    vertexBuffer->resize(num_vertices);
    auto vertexData = vertexBuffer->getVertexData();
    size_t vertexSize = vertexBuffer->getFormat().getVertexSize();
    size_t offset = vertexBuffer->getFormat().getVertexSize() / sizeof(float);

    static render::IndexArray ia;

    for (int i = 0; i < num_vertices; i++)
    {
        render::Vertex_2p2tx4c v;

        v.p.x = vertices[i].position.x;
        v.p.y = vertices[i].position.y;
        v.tx.x = vertices[i].tex_coord.x;
        v.tx.y = vertices[i].tex_coord.y;
        v.color.r = vertices[i].colour.red / 255.0f;
        v.color.g = vertices[i].colour.green / 255.0f;
        v.color.b = vertices[i].colour.blue / 255.0f;
        v.color.a = vertices[i].colour.alpha / 255.0f;

        // FIXME:
        // Map directly on the gpu.
        memcpy(vertexData + i * offset, &v, vertexSize);
    }

    for (int i = 0; i < num_indices; i++)
    {
        ia.push_back(indices[i]);
    }

    indexBuffer->appendIndices(ia);
    ia.clear();

    CompiledGeometry *geom = new CompiledGeometry();
    geom->texture = m_textures[texture];        // NOTE: can be nullptr. It's okay.
    geom->vb = vertexBuffer;
    geom->ib = indexBuffer;

    return (Rocket::Core::CompiledGeometryHandle)geom;
}

void GuiRenderInterface::RenderCompiledGeometry(Rocket::Core::CompiledGeometryHandle geometry,
                                                const Rocket::Core::Vector2f &translation)
{
    if (!geometry)
        return;

    auto geom = (CompiledGeometry *)geometry;

    m_guipass->setSampler("diffuseMap", geom->texture);

    render::RenderOperation op;
    op.vertexData = geom->vb;
    op.indexData = geom->ib;
    op.passProps = m_guipass;
    op.worldTransform = glm::translate(glm::vec3(translation.x, translation.y, 0.0f));

    g_renderManager->drawOperation(op);
}

void GuiRenderInterface::ReleaseCompiledGeometry(Rocket::Core::CompiledGeometryHandle geometry)
{
    auto geom = (CompiledGeometry *)geometry;
    delete geom;
}

void GuiRenderInterface::EnableScissorRegion(bool enable)
{
    g_renderManager->getRenderer()->enableScissorTest(enable);
}

void GuiRenderInterface::SetScissorRegion(int x, int y, int width, int height)
{
    g_renderManager->getRenderer()->setScissorRegion(x, g_renderManager->getViewport()->height() - (y + height), width, height);
}

bool GuiRenderInterface::LoadTexture(Rocket::Core::TextureHandle &texture_handle, 
                                     Rocket::Core::Vector2i &texture_dimensions, 
                                     const Rocket::Core::String &source)
{
    auto textureImage = g_resourceManager->getResource<render::Image>(source.CString());
    if (!textureImage || !textureImage->valid())
        return false;

    auto texture = make_shared<render::Texture>();

    texture->setType(render::Texture::TEXTURE_2D);
    texture->setFilteringMode(render::Texture::BILINEAR);
    texture->setMipmapped(false);

    texture->setImage(textureImage);

    m_textures[++m_textureId] = texture;
    texture_handle = m_textureId;

    texture_dimensions.x = textureImage->width();
    texture_dimensions.y = textureImage->height();

    return true;
}

bool GuiRenderInterface::GenerateTexture(Rocket::Core::TextureHandle& texture_handle,
                                         const Rocket::Core::byte *source,
                                         const Rocket::Core::Vector2i &source_dimensions)
{
    auto textureImage = make_shared<render::Image>();
    textureImage->setWithData(source, source_dimensions.x, source_dimensions.y, 4 * source_dimensions.x, render::Image::PF_RGBA);
    textureImage->setInitialized();

    auto texture = make_shared<render::Texture>();
    texture->setImage(textureImage);

    texture->setType(render::Texture::TEXTURE_2D);
    texture->setFilteringMode(render::Texture::BILINEAR);
    texture->setMipmapped(false);

    m_textures[++m_textureId] = texture;
    texture_handle = m_textureId;

    return true;
}

void GuiRenderInterface::ReleaseTexture(Rocket::Core::TextureHandle texture_handle)
{
    auto guid = m_textures[texture_handle]->getImage()->getGUID();

    // Erase from internal buffer.
    m_textures.erase(texture_handle);
    // Unload from resource manager.
    g_resourceManager->unloadResource(guid);
}

} }