#include "Sprite2DComponentProcessor.h"

#include <libdf3d/3d/SceneGraphComponentProcessor.h>
#include <libdf3d/game/ComponentDataHolder.h>
#include <libdf3d/game/World.h>
#include <libdf3d/render/RenderOperation.h>
#include <libdf3d/render/RenderQueue.h>
#include <libdf3d/render/RenderPass.h>
#include <libdf3d/render/Vertex.h>
#include <libdf3d/render/Texture.h>
#include <libdf3d/base/EngineController.h>
#include <libdf3d/resources/ResourceManager.h>
#include <libdf3d/resources/ResourceFactory.h>
#include <libdf3d/io/FileSystem.h>

namespace df3d {

struct Sprite2DComponentProcessor::Impl
{
    struct Data
    {
        RenderPass pass;
        RenderOperation2D op;
        glm::vec2 anchor = glm::vec2(0.5f, 0.5f);
        glm::vec2 textureOriginalSize;
        glm::vec2 screenPosition;
        ResourceGUID textureGuid;
        Entity holder;
        float rotation = 0.0f;
        bool visible = true;
    };

    ComponentDataHolder<Data> data;
    VertexBufferDescriptor vertexBuffer;

    Impl()
    {
        auto format = VertexFormat({ VertexFormat::POSITION_3, VertexFormat::TX_2, VertexFormat::COLOR_4 });

        // TODO_render
        //vertexData = createQuad2(format, 0.0f, 0.0f, 1.0, 1.0f, GpuBufferUsageType::STATIC);
    }

    ~Impl()
    {

    }

    static void updateTransform(Data &compData)
    {
        auto &worldTransform = compData.op.worldTransform;

        compData.op.z = worldTransform[3][2];

        if (compData.rotation != 0.0f)
        {
            auto r = glm::radians(compData.rotation);
            glm::mat2 m;
            m[0][0] = compData.textureOriginalSize.x * worldTransform[0][0];
            m[1][1] = compData.textureOriginalSize.y * worldTransform[1][1];

            m = glm::mat2(std::cos(r), -std::sin(r), std::sin(r), std::cos(r)) * m;

            worldTransform[0] = glm::vec4(m[0], 0.0f, 0.0f);
            worldTransform[1] = glm::vec4(m[1], 0.0f, 0.0f);
            worldTransform[2][2] = 1.0f;
        }
        else
        {
            // Set just quad scale.
            worldTransform[0][0] *= compData.textureOriginalSize.x;
            worldTransform[1][1] *= compData.textureOriginalSize.y;
            worldTransform[2][2] = 1.0f;
        }

        // Set position.
        worldTransform[3][0] += (0.5f - compData.anchor.x) * worldTransform[0][0];
        worldTransform[3][1] += (0.5f - compData.anchor.y) * worldTransform[1][1];
        worldTransform[3][2] = 0.0f;

        compData.screenPosition = { worldTransform[3][0], worldTransform[3][1] };
    }
};

void Sprite2DComponentProcessor::draw(RenderQueue *ops)
{
    // TODO:
    // Camera transform.
    for (auto &compData : m_pimpl->data.rawData())
    {
        if (!compData.visible)
            continue;

        Impl::updateTransform(compData);

        compData.op.vertexBuffer = m_pimpl->vertexBuffer;
        compData.op.passProps = &compData.pass;

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
        compData.op.worldTransform = m_world->sceneGraph().getWorldTransform(compData.holder);
}

Sprite2DComponentProcessor::Sprite2DComponentProcessor(World *world)
    : m_pimpl(new Impl()),
    m_world(world)
{

}

Sprite2DComponentProcessor::~Sprite2DComponentProcessor()
{
    //glog << "Sprite2DComponentProcessor::~Sprite2DComponentProcessor alive entities" << m_pimpl->data.rawData().size() << logdebug;
}

void Sprite2DComponentProcessor::setAnchorPoint(Entity e, const glm::vec2 &pt)
{
    m_pimpl->data.getData(e).anchor = pt;
}

void Sprite2DComponentProcessor::setZIdx(Entity e, float z)
{
    auto newPos = m_world->sceneGraph().getLocalPosition(e);
    newPos.z = z;

    m_world->sceneGraph().setPosition(e, newPos);
}

void Sprite2DComponentProcessor::setVisible(Entity e, bool visible)
{
    m_pimpl->data.getData(e).visible = visible;
}

void Sprite2DComponentProcessor::setSize(Entity e, const glm::vec2 &size)
{
    auto &compData = m_pimpl->data.getData(e);

    // Compute new scale to fit desired size.
    auto sc = size / compData.textureOriginalSize;
    m_world->sceneGraph().setScale(e, { sc.x, sc.y, 1.0f });
}

void Sprite2DComponentProcessor::setWidth(Entity e, float w)
{
    setSize(e, { w, getSize(e).y });
}

void Sprite2DComponentProcessor::setHeight(Entity e, float h)
{
    setSize(e, { getSize(e).x, h });
}

void Sprite2DComponentProcessor::setRotation(Entity e, float rotation)
{
    m_pimpl->data.getData(e).rotation = rotation;
}

glm::vec2 Sprite2DComponentProcessor::getSize(Entity e) const
{
    const auto &compData = m_pimpl->data.getData(e);
    auto scale = m_world->sceneGraph().getLocalScale(e);

    return{ scale.x * compData.textureOriginalSize.x, scale.y * compData.textureOriginalSize.y };
}

float Sprite2DComponentProcessor::getWidth(Entity e) const
{
    return getSize(e).x;
}

float Sprite2DComponentProcessor::getHeight(Entity e) const
{
    return getSize(e).y;
}

const glm::vec2& Sprite2DComponentProcessor::getScreenPosition(Entity e)
{
    auto &compData = m_pimpl->data.getData(e);

    Impl::updateTransform(compData);

    return compData.screenPosition;
}

void Sprite2DComponentProcessor::useTexture(Entity e, const std::string &pathToTexture)
{
    auto &compData = m_pimpl->data.getData(e);

    if (auto sampler = compData.pass.getTextureParam("diffuseMap"))
    {
        if (sampler->getFilePath() == svc().fileSystem().fullPath(pathToTexture))
            return;
    }

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

    if (compData.textureGuid == texture->getGUID())
        return;

    compData.pass.setParam("diffuseMap", texture);
    compData.textureOriginalSize = { texture->getWidth(), texture->getHeight() };
    compData.textureGuid = texture->getGUID();
}

const glm::vec2& Sprite2DComponentProcessor::getTextureSize(Entity e) const
{
    return m_pimpl->data.getData(e).textureOriginalSize;
}

void Sprite2DComponentProcessor::setBlendMode(Entity e, RenderPass::BlendingMode bm)
{
    m_pimpl->data.getData(e).pass.setBlendMode(bm);
}

void Sprite2DComponentProcessor::setBlendMode2(Entity e, int bm)
{
    setBlendMode(e, static_cast<RenderPass::BlendingMode>(bm));
}

void Sprite2DComponentProcessor::setDiffuseColor(Entity e, const glm::vec4 &diffuseColor)
{
    m_pimpl->data.getData(e).pass.setParam("material_diffuse", diffuseColor);
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
    data.pass.setFaceCullMode(RenderPass::FaceCullMode::NONE);
    data.pass.setGpuProgram(svc().resourceManager().getFactory().createColoredGpuProgram());
    data.pass.enableDepthTest(false);
    data.pass.enableDepthWrite(false);
    data.pass.setBlendMode(RenderPass::BlendingMode::ALPHA);
    data.op.worldTransform = m_world->sceneGraph().getWorldTransform(e);

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
