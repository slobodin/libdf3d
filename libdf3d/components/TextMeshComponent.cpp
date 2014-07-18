#include "df3d_pch.h"
#include "TextMeshComponent.h"

#include <render/Texture.h>
#include <render/RenderPass.h>
#include <render/GpuProgram.h>
#include <render/VertexIndexBuffer.h>
#include <render/RenderQueue.h>
#include <render/Image.h>
#include <gui/FontFace.h>
#include <base/Controller.h>
#include <resources/ResourceManager.h>

namespace df3d { namespace components {

shared_ptr<render::RenderPass> TextMeshComponent::createRenderPass()
{
    auto pass = make_shared<render::RenderPass>("text_mesh_render_pass");
    pass->setFrontFaceWinding(render::RenderPass::WO_CCW);
    pass->setFaceCullMode(render::RenderPass::FCM_NONE);
    pass->setPolygonDrawMode(render::RenderPass::PM_FILL);
    pass->setDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
    pass->setBlendMode(render::RenderPass::BM_NONE);

    auto program = g_resourceManager->getResource<render::GpuProgram>(render::COLORED_PROGRAM_EMBED_PATH);
    pass->setGpuProgram(program);

    return pass;
}

void TextMeshComponent::onDraw(render::RenderQueue *ops)
{
    if (m_op.passProps->isTransparent())
        ops->transparentOperations.push_back(m_op);
    else
        ops->notLitOpaqueOperations.push_back(m_op);
}

TextMeshComponent::TextMeshComponent(const char *fontPath)
    : MeshComponent()
{
    auto font = g_resourceManager->getResource<gui::FontFace>(fontPath);
    if (!font)
    {
        base::glog << "Failed to initialize text renderer. Font is invalid" << base::logwarn;
        return;
    }

    m_font = font;

    // Init render operation.
    m_op.passProps = createRenderPass();
    m_op.vertexData = render::createQuad(render::VertexFormat::create("p:3, tx:2, c:4"), 0.0f, 0.0f, 2.0f, 2.0f);
    m_op.vertexData->setUsageType(render::GB_USAGE_STATIC);
}

void TextMeshComponent::drawText(const char *text, const glm::vec3 &color, int size)
{
    if (!m_font)
        return;

    //auto textureImage = make_shared<render::Image>();
    //textureImage->setWithData(surf);
    //textureImage->setInitialized();

    //auto texture = make_shared<render::Texture>();
    //texture->setImage(textureImage);

    //texture->setType(render::Texture::TEXTURE_2D);
    //texture->setWrapMode(render::Texture::WM_CLAMP);
    //texture->setFilteringMode(render::Texture::BILINEAR);
    //texture->setMipmapped(false);

    //m_op.passProps->setSampler("diffuseMap", texture);
}

shared_ptr<NodeComponent> TextMeshComponent::clone() const
{
    // Not implemented.
    assert(false);
    return nullptr;
}

} }