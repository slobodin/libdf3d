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
// Camera transform.

void Sprite2DComponent::onDraw(render::RenderQueue *ops)
{
    m_op.worldTransform = getHolder()->transform()->getTransformation();
    m_op.z = m_op.worldTransform[3][2];

    auto size = getSize();
    m_op.worldTransform[3][0] += (0.5f - m_anchor.x) * size.x;
    m_op.worldTransform[3][1] += (0.5f - m_anchor.y) * size.y;
    m_op.worldTransform[3][2] = 0.0f;
    ops->sprite2DOperations.push_back(m_op);
}

void Sprite2DComponent::onAttached()
{
    // FIXME: implicitly sets transformation.
    setSize({ getTextureWidth(), getTextureHeight() });
}

Sprite2DComponent::Sprite2DComponent(const char *pathToTexture)
    : NodeComponent(SPRITE_2D)
{
    auto sprite2dPass = make_shared<render::RenderPass>();
    sprite2dPass->setFaceCullMode(render::RenderPass::FaceCullMode::NONE);
    sprite2dPass->setGpuProgram(g_resourceManager->createFFP2DGpuProgram());
    sprite2dPass->enableDepthTest(false);
    sprite2dPass->enableDepthWrite(false);
    sprite2dPass->setBlendMode(render::RenderPass::BlendingMode::NONE);

    auto texture = g_resourceManager->createTexture(pathToTexture, ResourceLoadingMode::IMMEDIATE);
    if (!texture || !texture->valid())
        base::glog << "Failed to init Sprite2DComponent with texture" << pathToTexture << base::logwarn;

    texture->setFilteringMode(render::TextureFiltering::BILINEAR);
    texture->setMipmapped(false);
    texture->setMaxAnisotropy(render::NO_ANISOTROPY);

    sprite2dPass->setSampler("diffuseMap", texture);

    auto quadVb = render::createQuad2(render::VertexFormat::create("p:2, tx:2, c:4"), 0.0f, 0.0f, 1.0, 1.0f);
    quadVb->setUsageType(render::GpuBufferUsageType::STATIC);

    m_op.passProps = sprite2dPass;
    m_op.vertexData = quadVb;
}

Sprite2DComponent::~Sprite2DComponent()
{

}

void Sprite2DComponent::setAnchorPoint(const glm::vec2 &pt)
{
    m_anchor = pt;
}

void Sprite2DComponent::setSize(const glm::vec2 &v)
{
    getHolder()->transform()->setScale(v.x, v.y, 1.0f);
}

void Sprite2DComponent::setWidth(float w)
{
    auto sz = getSize();
    getHolder()->transform()->setScale(w, sz.y, 1.0f);
}

void Sprite2DComponent::setHeight(float h)
{
    auto sz = getSize();
    getHolder()->transform()->setScale(sz.x, h, 1.0f);
}

glm::vec2 Sprite2DComponent::getSize()
{
    return glm::vec2(getHolder()->transform()->getScale());
}

float Sprite2DComponent::getWidth()
{
    return getSize().x;
}

float Sprite2DComponent::getHeight()
{
    return getSize().y;
}

size_t Sprite2DComponent::getTextureWidth() const
{
    return static_pointer_cast<render::Texture2D>(m_op.passProps->getSampler("diffuseMap"))->getOriginalWidth();
}

size_t Sprite2DComponent::getTextureHeight() const
{
    return static_pointer_cast<render::Texture2D>(m_op.passProps->getSampler("diffuseMap"))->getOriginalHeight();
}

void Sprite2DComponent::setBlendMode(render::RenderPass::BlendingMode bm)
{
    m_op.passProps->setBlendMode(bm);
}

void Sprite2DComponent::setDiffuseColor(const glm::vec4 &diffuseColor)
{
    m_op.passProps->setDiffuseColor(diffuseColor);
}

shared_ptr<NodeComponent> Sprite2DComponent::clone() const
{
    // TODO:
    assert(false);

    return nullptr;
}

} }
