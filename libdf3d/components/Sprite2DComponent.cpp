#include "df3d_pch.h"
#include "Sprite2DComponent.h"

#include <render/RenderPass.h>
#include <render/RenderQueue.h>
#include <render/VertexIndexBuffer.h>
#include <render/Texture2D.h>
#include <components/TransformComponent.h>
#include <scene/Node.h>
#include <base/SystemsMacro.h>

namespace df3d { namespace components {

// TODO:
// Transform component attached - set width & height

void Sprite2DComponent::onDraw(render::RenderQueue *ops)
{
    assert(getHolder()->transform());

    m_op.worldTransform = getHolder()->transform()->getTransformation();
    ops->sprite2DOperations.push_back(m_op);
}

Sprite2DComponent::Sprite2DComponent(const char *pathToTexture)
    : NodeComponent(SPRITE_2D)
{
    auto sprite2dPass = make_shared<render::RenderPass>();
    sprite2dPass->setFaceCullMode(render::RenderPass::FaceCullMode::NONE);
    sprite2dPass->setGpuProgram(g_resourceManager->createFFP2DGpuProgram());
    sprite2dPass->enableDepthTest(false);
    sprite2dPass->enableDepthWrite(false);
    sprite2dPass->setBlendMode(render::RenderPass::BlendingMode::ALPHA);

    auto texture = g_resourceManager->createTexture(pathToTexture, ResourceLoadingMode::IMMEDIATE);
    if (!texture || !texture->valid())
        base::glog << "Failed to init Sprite2DComponent with texture" << pathToTexture << base::logwarn;

    texture->setFilteringMode(render::TextureFiltering::BILINEAR);
    texture->setMipmapped(false);
    texture->setMaxAnisotropy(render::NO_ANISOTROPY);

    sprite2dPass->setSampler("diffuseMap", texture);

    auto quadVb = render::createQuad2(render::VertexFormat::create("p:2, tx:2, c:4"), 0.0f, 0.0f, 2.0, 2.0f);
    quadVb->setUsageType(render::GpuBufferUsageType::STATIC);

    m_op.passProps = sprite2dPass;
    m_op.vertexData = quadVb;
}

Sprite2DComponent::~Sprite2DComponent()
{

}

shared_ptr<NodeComponent> Sprite2DComponent::clone() const
{
    // TODO:
    assert(false);

    return nullptr;
}

} }
