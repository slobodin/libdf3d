#include "Sprite2DComponentProcessor.h"

#include <3d/TransformComponentProcessor.h>
#include <game/ComponentDataHolder.h>
#include <game/World.h>
#include <render/RenderOperation.h>
#include <render/RenderQueue.h>
#include <render/VertexIndexBuffer.h>
#include <render/Texture2D.h>
#include <base/EngineController.h>
#include <resources/ResourceManager.h>
#include <resources/ResourceFactory.h>

namespace df3d {

struct Sprite2DComponentProcessor::Impl
{
    struct Data
    {
        Entity holder;

        RenderOperation2D op;

        glm::vec2 anchor = glm::vec2(0.5f, 0.5f);
        glm::vec2 textureOriginalSize;
        glm::vec2 screenPosition;

        ResourceGUID textureGuid;
    };

    ComponentDataHolder<Data> data;
};

void Sprite2DComponentProcessor::draw(RenderQueue *ops)
{
    // TODO:
    // Camera transform.
    for (auto &compData : m_pimpl->data.rawData())
    {
        auto &worldTransform = compData.op.worldTransform;

        compData.op.z = worldTransform[3][2];

        // Set quad scale.
        worldTransform[0][0] *= compData.textureOriginalSize.x;
        worldTransform[1][1] *= compData.textureOriginalSize.y;
        worldTransform[2][2] = 1.0f;

        // Set position.
        worldTransform[3][0] += (0.5f - compData.anchor.x) * worldTransform[0][0];
        worldTransform[3][1] += (0.5f - compData.anchor.y) * worldTransform[1][1];
        worldTransform[3][2] = 0.0f;

        compData.screenPosition = { worldTransform[3][0], worldTransform[3][1] };

        ops->sprite2DOperations.push_back(compData.op);
    }
}

void Sprite2DComponentProcessor::cleanStep(const std::list<Entity> &deleted)
{
    m_pimpl->data.cleanStep(deleted);
}

void Sprite2DComponentProcessor::update()
{
    for (auto &compData : m_pimpl->data.rawData())
        compData.op.worldTransform = m_world->transform().getTransformation(compData.holder);
}

Sprite2DComponentProcessor::Sprite2DComponentProcessor(World *world)
    : m_pimpl(new Impl()),
    m_world(world)
{

}

Sprite2DComponentProcessor::~Sprite2DComponentProcessor()
{
    glog << "Sprite2DComponentProcessor::~Sprite2DComponentProcessor alive entities" << m_pimpl->data.rawData().size() << logdebug;
}

void Sprite2DComponentProcessor::setAnchorPoint(Entity e, const glm::vec2 &pt)
{
    m_pimpl->data.getData(e).anchor = pt;
}

void Sprite2DComponentProcessor::setZIdx(Entity e, float z)
{
    auto newPos = m_world->transform().getPosition(e);
    newPos.z = z;

    m_world->transform().setPosition(e, newPos);
    m_pimpl->data.getData(e).op.worldTransform = m_world->transform().getTransformation(e);
}

void Sprite2DComponentProcessor::setSize(Entity e, const glm::vec2 &size)
{
    auto &compData = m_pimpl->data.getData(e);

    // Compute new scale to fit desired size.
    auto sc = size / compData.textureOriginalSize;
    m_world->transform().setScale(e, sc.x, sc.y, 1.0f);

    compData.op.worldTransform = m_world->transform().getTransformation(e);
}

void Sprite2DComponentProcessor::setWidth(Entity e, float w)
{
    setSize(e, { w, getSize(e).y });
}

void Sprite2DComponentProcessor::setHeight(Entity e, float h)
{
    setSize(e, { getSize(e).x, h });
}

glm::vec2 Sprite2DComponentProcessor::getSize(Entity e)
{
    const auto &compData = m_pimpl->data.getData(e);
    auto &tr = compData.op.worldTransform;

    return{ tr[0][0] * compData.textureOriginalSize.x, tr[1][1] * compData.textureOriginalSize.y };
}

float Sprite2DComponentProcessor::getWidth(Entity e)
{
    return getSize(e).x;
}

float Sprite2DComponentProcessor::getHeight(Entity e)
{
    return getSize(e).y;
}

glm::vec2 Sprite2DComponentProcessor::getScreenPosition(Entity e) const
{
    return m_pimpl->data.getData(e).screenPosition;
}

void Sprite2DComponentProcessor::useTexture(Entity e, const std::string &pathToTexture)
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

    auto &compData = m_pimpl->data.getData(e);

    if (compData.textureGuid == texture->getGUID())
        return;

    compData.op.passProps->setSampler("diffuseMap", texture);
    compData.textureOriginalSize = { texture->getOriginalWidth(), texture->getOriginalHeight() };
    compData.textureGuid = texture->getGUID();
}

glm::vec2 Sprite2DComponentProcessor::getTextureSize(Entity e) const
{
    return m_pimpl->data.getData(e).textureOriginalSize;
}

void Sprite2DComponentProcessor::setBlendMode(Entity e, RenderPass::BlendingMode bm)
{
    m_pimpl->data.getData(e).op.passProps->setBlendMode(bm);
}

void Sprite2DComponentProcessor::setDiffuseColor(Entity e, const glm::vec4 &diffuseColor)
{
    m_pimpl->data.getData(e).op.passProps->setDiffuseColor(diffuseColor);
}

void Sprite2DComponentProcessor::add(Entity e, const std::string &texturePath)
{
    if (m_pimpl->data.contains(e))
    {
        glog << "An entity already has a sprite2d component" << logwarn;
        return;
    }

    Impl::Data data;

    data.holder = e;
    data.op.passProps = make_shared<RenderPass>();
    data.op.passProps->setFaceCullMode(RenderPass::FaceCullMode::NONE);
    data.op.passProps->setGpuProgram(svc().resourceManager().getFactory().createColoredGpuProgram());
    data.op.passProps->enableDepthTest(false);
    data.op.passProps->enableDepthWrite(false);
    data.op.passProps->setBlendMode(RenderPass::BlendingMode::ALPHA);
    data.op.worldTransform = m_world->transform().getTransformation(e);

    auto format = VertexFormat({ VertexFormat::POSITION_3, VertexFormat::TX_2, VertexFormat::COLOR_4 });
    data.op.vertexData = createQuad2(format, 0.0f, 0.0f, 1.0, 1.0f, GpuBufferUsageType::STATIC);

    m_pimpl->data.add(e, data);

    useTexture(e, texturePath);
}

void Sprite2DComponentProcessor::remove(Entity e)
{
    if (!m_pimpl->data.contains(e))
    {
        glog << "Failed to remove sprite2 component from an entity. Component is not attached" << logwarn;
        return;
    }

    m_pimpl->data.remove(e);
}

bool Sprite2DComponentProcessor::has(Entity e)
{
    return m_pimpl->data.lookup(e).valid();
}

}
