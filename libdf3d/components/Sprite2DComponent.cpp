#include "Sprite2DComponent.h"

#include <render/RenderPass.h>
#include <render/RenderQueue.h>
#include <render/VertexIndexBuffer.h>
#include <render/Texture2D.h>
#include <components/TransformComponent.h>
#include <scene/Node.h>
#include <base/EngineController.h>
#include <resources/ResourceManager.h>
#include <resources/ResourceFactory.h>

namespace df3d {

// TODO:
// Camera transform.

void Sprite2DComponent::onDraw(RenderQueue *ops)
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

    m_screenPosition = { m_op.worldTransform[3][0], m_op.worldTransform[3][1] };

    ops->sprite2DOperations.push_back(m_op);
}

Sprite2DComponent::Sprite2DComponent(const std::string &pathToTexture)
    : NodeComponent(ComponentType::SPRITE_2D)
{
    m_op.passProps = make_shared<RenderPass>();
    m_op.passProps->setFaceCullMode(RenderPass::FaceCullMode::NONE);
    m_op.passProps->setGpuProgram(svc().resourceManager().getFactory().createColoredGpuProgram());
    m_op.passProps->enableDepthTest(false);
    m_op.passProps->enableDepthWrite(false);
    m_op.passProps->setBlendMode(RenderPass::BlendingMode::ALPHA);

    auto format = VertexFormat({ VertexFormat::POSITION_3, VertexFormat::TX_2, VertexFormat::COLOR_4 });
    m_op.vertexData = createQuad2(format, 0.0f, 0.0f, 1.0, 1.0f, GpuBufferUsageType::STATIC);

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
    auto tr = getHolder()->transform()->getTransformation();

    return { tr[0][0] * m_textureOriginalSize.x, tr[1][1] * m_textureOriginalSize.y };
}

float Sprite2DComponent::getWidth()
{
    return getSize().x;
}

float Sprite2DComponent::getHeight()
{
    return getSize().y;
}

glm::vec2 Sprite2DComponent::getScreenPosition() const
{
    return m_screenPosition;
}

void Sprite2DComponent::useTexture(const std::string &pathToTexture)
{
    TextureCreationParams params;
    params.setFiltering(TextureFiltering::BILINEAR);
    params.setMipmapped(false);
    params.setAnisotropyLevel(NO_ANISOTROPY);

    auto texture = svc().resourceManager().getFactory().createTexture(pathToTexture, params, ResourceLoadingMode::IMMEDIATE);
    if (!texture || !texture->isInitialized())
    {
        glog << "Failed to init Sprite2DComponent with texture" << pathToTexture << logwarn;
        return;
    }

    if (m_textureGuid == texture->getGUID())
        return;

    m_op.passProps->setSampler("diffuseMap", texture);
    m_textureOriginalSize = { texture->getOriginalWidth(), texture->getOriginalHeight() };
    m_textureGuid = texture->getGUID();
}

glm::vec2 Sprite2DComponent::getTextureSize() const
{
    return m_textureOriginalSize;
}

void Sprite2DComponent::setBlendMode(RenderPass::BlendingMode bm)
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

}
