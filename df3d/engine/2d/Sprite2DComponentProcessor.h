#pragma once

#include <df3d/lib/Id.h>
#include <df3d/game/EntityComponentProcessor.h>
#include <df3d/game/ComponentDataHolder.h>
#include <df3d/engine/render/Material.h>
#include <df3d/engine/render/RenderOperation.h>

namespace df3d {

struct RenderQueue;
class World;
class SceneGraphComponentProcessor;

// FIXME: improve 2d submodule, ideally remove this class.

class Sprite2DComponentProcessor : public EntityComponentProcessor
{
    friend class World;

    struct Data
    {
        RenderPass pass;
        RenderOperation op;
        glm::vec2 anchor = glm::vec2(0.5f, 0.5f);
        glm::vec2 textureOriginalSize;
        glm::vec2 screenPosition;
        Id textureResourceId;
        Entity holder;
        float rotation = 0.0f;
        bool visible = true;
    };

    ComponentDataHolder<Data> m_data;
    VertexBufferHandle m_vertexBuffer;

    World &m_world;

    void updateTransform(Data &compData, SceneGraphComponentProcessor &sceneGr);

    void draw(RenderQueue *ops) override;
    void update() override { }

public:
    Sprite2DComponentProcessor(World &world);
    ~Sprite2DComponentProcessor();

    void setAnchorPoint(Entity e, const glm::vec2 &pt);
    void setZIdx(Entity e, float z);
    void setVisible(Entity e, bool visible);

    void setSize(Entity e, const glm::vec2 &size);
    void setWidth(Entity e, float w);
    void setHeight(Entity e, float h);
    void setRotation(Entity e, float rotation);

    glm::vec2 getSize(Entity e) const;
    float getWidth(Entity e) const;
    float getHeight(Entity e) const;

    const glm::vec2& getScreenPosition(Entity e);

    void useTexture(Entity e, Id textureResource);
    const glm::vec2& getTextureSize(Entity e) const;

    void setBlending(Entity e, Blending blending);
    void setDiffuseColor(Entity e, const glm::vec4 &diffuseColor);

    void add(Entity e, Id textureResource);
    void remove(Entity e) override;
    bool has(Entity e) override;
};

}
