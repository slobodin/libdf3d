#pragma once

#include <libdf3d/game/Entity.h>
#include <libdf3d/game/EntityComponentProcessor.h>
#include <libdf3d/render/RenderPass.h>

namespace df3d {

class RenderQueue;
class World;

// FIXME: improve 2d submodule, ideally remove this class.

class DF3D_DLL Sprite2DComponentProcessor : public EntityComponentProcessor
{
    friend class World;

    struct Impl;
    unique_ptr<Impl> m_pimpl;
    World *m_world;

    void draw(RenderQueue *ops);
    void cleanStep(const std::list<Entity> &deleted) override;
    void update() override;

public:
    Sprite2DComponentProcessor(World *world);
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

    void useTexture(Entity e, const std::string &pathToTexture);
    const glm::vec2& getTextureSize(Entity e) const;

    void setBlendMode(Entity e, RenderPass::BlendingMode bm);
    // FIXME:
    void setBlendMode2(Entity e, int bm);
    void setDiffuseColor(Entity e, const glm::vec4 &diffuseColor);

    void add(Entity e, const std::string &texturePath);
    void remove(Entity e);
    bool has(Entity e);
};

}
