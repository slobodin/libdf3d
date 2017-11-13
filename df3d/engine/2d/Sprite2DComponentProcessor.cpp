#include "Sprite2DComponentProcessor.h"

#include <df3d/engine/3d/SceneGraphComponentProcessor.h>
#include <df3d/game/ComponentDataHolder.h>
#include <df3d/game/World.h>
#include <df3d/engine/render/RenderOperation.h>
#include <df3d/engine/render/RenderManager.h>
#include <df3d/engine/render/IRenderBackend.h>
#include <df3d/engine/render/RenderQueue.h>
#include <df3d/engine/render/Material.h>
#include <df3d/engine/render/Vertex.h>
#include <df3d/engine/EngineController.h>
#include <df3d/engine/resources/ResourceManager.h>
#include <df3d/engine/resources/TextureResource.h>

namespace df3d {

static VertexBufferHandle CreateQuad(float x, float y, float w, float h)
{
    const float w2 = w / 2.0f;
    const float h2 = h / 2.0f;
    const glm::vec4 color = { 1.0f, 1.0f, 1.0f, 1.0f };

    Vertex_p_tx_c quadData[6] = {
        { { x - w2, y - h2, 0.0f }, { 0.0, 0.0 }, color },
        { { x + w2, y - h2, 0.0f }, { 1.0, 0.0 }, color },
        { { x + w2, y + h2, 0.0f }, { 1.0, 1.0 }, color },
        { { x + w2, y + h2, 0.0f }, { 1.0, 1.0 }, color },
        { { x - w2, y + h2, 0.0f }, { 0.0, 1.0 }, color },
        { { x - w2, y - h2, 0.0f }, { 0.0, 0.0 }, color },
    };

    return svc().renderManager().getBackend().createStaticVertexBuffer(Vertex_p_tx_c::getFormat(), 6, quadData);
}

void Sprite2DComponentProcessor::updateTransform(Data &compData, SceneGraphComponentProcessor &sceneGr)
{
    compData.op.worldTransform = sceneGr.getWorldTransformMatrix(compData.holder);

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

void Sprite2DComponentProcessor::draw(RenderQueue *ops)
{
    if (!m_vertexBuffer.isValid())
        m_vertexBuffer = CreateQuad(0.0f, 0.0f, 1.0, 1.0f);
    // TODO:
    // Camera transform.
    auto &sceneGr = m_world.sceneGraph();
    for (auto &compData : m_data.rawData())
    {
        if (!compData.visible)
            continue;

        updateTransform(compData, sceneGr);

        compData.op.vertexBuffer = m_vertexBuffer;
        compData.op.passProps = &compData.pass;
        compData.op.numberOfElements = 6;       // This is a quad! Two triangles.

        ops->rops[RQ_BUCKET_2D].push_back(compData.op);
    }
}

Sprite2DComponentProcessor::Sprite2DComponentProcessor(World &world)
    : m_world(world)
{

}

Sprite2DComponentProcessor::~Sprite2DComponentProcessor()
{
    if (m_vertexBuffer.isValid())
        svc().renderManager().getBackend().destroyVertexBuffer(m_vertexBuffer);
}

void Sprite2DComponentProcessor::setAnchorPoint(Entity e, const glm::vec2 &pt)
{
    m_data.getData(e).anchor = pt;
}

void Sprite2DComponentProcessor::setZIdx(Entity e, float z)
{
    auto newPos = m_world.sceneGraph().getLocalPosition(e);
    newPos.z = z;

    m_world.sceneGraph().setPosition(e, newPos);
}

void Sprite2DComponentProcessor::setVisible(Entity e, bool visible)
{
    m_data.getData(e).visible = visible;

    const auto &children = m_world.sceneGraph().getChildren(e);
    for (auto child : children)
    {
        if (has(child))
            setVisible(child, visible);
    }
}

void Sprite2DComponentProcessor::setSize(Entity e, const glm::vec2 &size)
{
    auto &compData = m_data.getData(e);

    // Compute new scale to fit desired size.
    auto sc = size / compData.textureOriginalSize;
    m_world.sceneGraph().setScale(e, { sc.x, sc.y, 1.0f });
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
    m_data.getData(e).rotation = rotation;
}

glm::vec2 Sprite2DComponentProcessor::getSize(Entity e) const
{
    const auto &compData = m_data.getData(e);
    auto scale = m_world.sceneGraph().getLocalScale(e);

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
    auto &compData = m_data.getData(e);

    updateTransform(compData, m_world.sceneGraph());

    return compData.screenPosition;
}

void Sprite2DComponentProcessor::useTexture(Entity e, Id textureResource)
{
    auto &compData = m_data.getData(e);
    if (compData.textureResourceId == textureResource)
        return;

    auto texture = svc().resourceManager().getResource<TextureResource>(textureResource);
    if (!texture)
    {
        DFLOG_WARN("Failed to init Sprite2DComponent with texture %s", textureResource.toString().c_str());
        return;
    }

    compData.pass.setParam(Id("diffuseMap"), texture->handle);
    compData.textureOriginalSize = { texture->width, texture->height };
    compData.textureResourceId = textureResource;
}

const glm::vec2& Sprite2DComponentProcessor::getTextureSize(Entity e) const
{
    return m_data.getData(e).textureOriginalSize;
}

void Sprite2DComponentProcessor::setBlending(Entity e, Blending blending)
{
    m_data.getData(e).pass.setBlending(blending);
}

void Sprite2DComponentProcessor::setDiffuseColor(Entity e, const glm::vec4 &diffuseColor)
{
    auto &compData = m_data.getData(e);
    compData.pass.setParam(Id("material_diffuse"), diffuseColor);
}

void Sprite2DComponentProcessor::add(Entity e, Id textureResource)
{
    DF3D_ASSERT_MESS(!m_data.contains(e), "An entity already has sprite2d component");

    Data data;

    data.holder = e;

    data.pass.setDepthTest(false);
    data.pass.setDepthWrite(false);
    data.pass.setBlending(Blending::ALPHA);
    data.pass.setBackFaceCullingEnabled(false);
    data.pass.setParam(Id("material_diffuse"), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

    data.op.worldTransform = m_world.sceneGraph().getWorldTransformMatrix(e);

    m_data.add(e, data);

    useTexture(e, textureResource);

    auto gpuProgram = svc().renderManager().getEmbedResources().coloredProgram;
    m_data.getData(e).pass.program = gpuProgram;
}

void Sprite2DComponentProcessor::remove(Entity e)
{
    m_data.remove(e);
}

bool Sprite2DComponentProcessor::has(Entity e)
{
    return m_data.contains(e);
}

}
