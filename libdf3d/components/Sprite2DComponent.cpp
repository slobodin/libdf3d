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

    // Set quad scale.
    m_op.worldTransform[0][0] *= m_textureOriginalSize.x;
    m_op.worldTransform[1][1] *= m_textureOriginalSize.y;
    m_op.worldTransform[2][2] = 1.0f;

    // Set position.
    m_op.worldTransform[3][0] += (0.5f - m_anchor.x) * m_op.worldTransform[0][0];
    m_op.worldTransform[3][1] += (0.5f - m_anchor.y) * m_op.worldTransform[1][1];
    m_op.worldTransform[3][2] = 0.0f;
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

    auto quadVb = render::createQuad2(render::VertexFormat::create("p:2, tx:2, c:4"), 0.0f, 0.0f, 1.0, 1.0f);
    quadVb->setUsageType(render::GpuBufferUsageType::STATIC);

    m_op.passProps = sprite2dPass;
    m_op.vertexData = quadVb;

    useTexture(pathToTexture);
}

Sprite2DComponent::~Sprite2DComponent()
{

}

void Sprite2DComponent::setAnchorPoint(const glm::vec2 &pt)
{
    m_anchor = pt;
}

void Sprite2DComponent::setZIdx(float z)
{
    auto newPos = getHolder()->transform()->getPosition();
    newPos.z = z;

    getHolder()->transform()->setPosition(newPos);
}

void Sprite2DComponent::setSize(const glm::vec2 &size)
{
    // Compute new scale to fit desired size.
    auto sc = size / m_textureOriginalSize;
    getHolder()->transform()->setScale(sc.x, sc.y, 1.0f);
}

void Sprite2DComponent::setWidth(float w)
{
    setSize({ w, getSize().y });
}

void Sprite2DComponent::setHeight(float h)
{
    setSize({ getSize().x, h });
}

glm::vec2 Sprite2DComponent::getSize()
{
    assert(false);
    auto scale = getHolder()->transform()->getScale();
    return glm::vec2(m_textureOriginalSize.x * scale.x, m_textureOriginalSize.y * scale.y);
}

float Sprite2DComponent::getWidth()
{
    return getSize().x;
}

float Sprite2DComponent::getHeight()
{
    return getSize().y;
}

void Sprite2DComponent::useTexture(const char *pathToTexture)
{
    auto texture = g_resourceManager->createTexture(pathToTexture, ResourceLoadingMode::IMMEDIATE);
    if (!texture || !texture->valid())
        base::glog << "Failed to init Sprite2DComponent with texture" << pathToTexture << base::logwarn;

    texture->setFilteringMode(render::TextureFiltering::BILINEAR);
    texture->setMipmapped(false);
    texture->setMaxAnisotropy(render::NO_ANISOTROPY);

    m_op.passProps->setSampler("diffuseMap", texture);
    m_textureOriginalSize = { texture->getOriginalWidth(), texture->getOriginalHeight() };
}

glm::vec2 Sprite2DComponent::getTextureSize() const
{
    return m_textureOriginalSize;
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
