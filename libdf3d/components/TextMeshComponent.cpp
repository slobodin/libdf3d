#include "df3d_pch.h"
#include "TextMeshComponent.h"

#include <render/Texture.h>
#include <render/RenderPass.h>
#include <render/GpuProgram.h>
#include <render/VertexIndexBuffer.h>
#include <render/RenderQueue.h>
#include <render/Image.h>
#include <base/Controller.h>
#include <resources/ResourceManager.h>
#include <resources/FileSystem.h>
#include <SDL_ttf.h>

namespace df3d { namespace components {

shared_ptr<render::RenderPass> TextMeshComponent::createRenderPass()
{
    auto pass = make_shared<render::RenderPass>("text_mesh_render_pass");
    pass->setFrontFaceWinding(render::RenderPass::WO_CCW);
    pass->setFaceCullMode(render::RenderPass::FCM_NONE);
    pass->setPolygonDrawMode(render::RenderPass::PM_FILL);
    pass->setDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
    pass->setBlendMode(render::RenderPass::BM_ALPHA);

    auto program = g_resourceManager->getResource<render::GpuProgram>(render::COLORED_PROGRAM_EMBED_PATH);
    pass->setGpuProgram(program);

    return pass;
}

void TextMeshComponent::onDraw(render::RenderQueue *ops)
{
    if (!m_font)
        return;

    if (m_op.passProps->isTransparent())
        ops->transparentOperations.push_back(m_op);
    else
        ops->notLitOpaqueOperations.push_back(m_op);
}

TextMeshComponent::TextMeshComponent(const char *fontPath)
    : MeshComponent()
{
    // FIXME:
    // For now without resource manager.
    auto path = g_fileSystem->fullPath(fontPath);
    m_font = TTF_OpenFont(path.c_str(), 18);
    if (!m_font)
    {
        base::glog << "Failed to initialize text renderer. Font is invalid" << TTF_GetError() << base::logwarn;
        return;
    }

    // Init render operation.
    m_op.passProps = createRenderPass();
    m_op.vertexData = render::createQuad(render::VertexFormat::create("p:3, tx:2"), 0.0f, 0.0f, 2.0f, 2.0f);
    m_op.vertexData->setUsageType(render::GB_USAGE_STATIC);
}

TextMeshComponent::~TextMeshComponent()
{
    if (m_font)
        TTF_CloseFont(m_font);
}

void TextMeshComponent::drawText(const char *text, const glm::vec3 &color, int size)
{
    if (!m_font)
        return;

    if (size == 0)
    {
        // TODO:
        // Choose default.
        return;
    }

    SDL_Color color1 = { 255, 255, 255, 255 };
    SDL_Surface *text_surface = TTF_RenderText_Blended(m_font, text, color1);

    auto textureImage = make_shared<render::Image>();
    textureImage->setWithData(text_surface);
    textureImage->setInitialized();

    SDL_FreeSurface(text_surface);

    auto texture = make_shared<render::Texture>();
    texture->setImage(textureImage);

    texture->setType(render::Texture::TEXTURE_2D);
    texture->setWrapMode(render::Texture::WM_CLAMP);
    texture->setFilteringMode(render::Texture::BILINEAR);
    texture->setMipmapped(false);

    m_op.passProps->setSampler("diffuseMap", texture);
}

shared_ptr<NodeComponent> TextMeshComponent::clone() const
{
    // Not implemented.
    assert(false);
    return nullptr;
}

} }