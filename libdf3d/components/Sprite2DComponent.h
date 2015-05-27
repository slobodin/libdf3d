#pragma once

#include "NodeComponent.h"
#include <render/RenderOperation.h>
#include <render/RenderPass.h>

namespace df3d { namespace components {

class DF3D_DLL Sprite2DComponent : public NodeComponent
{
    render::RenderOperation2D m_op;

    glm::vec2 m_anchor = glm::vec2(0.5f, 0.5f);

    virtual void onDraw(render::RenderQueue *ops) override;
    virtual void onAttached() override;

public:
    Sprite2DComponent(const char *pathToTexture);
    ~Sprite2DComponent();

    void setAnchorPoint(const glm::vec2 &pt);

    void setSize(const glm::vec2 &v);
    void setWidth(float w);
    void setHeight(float h);

    glm::vec2 getSize();
    float getWidth();
    float getHeight();

    size_t getTextureWidth() const;
    size_t getTextureHeight() const;

    void setBlendMode(render::RenderPass::BlendingMode bm);

    // TODO:
    // Set anchor
    // Set width & height

    virtual shared_ptr<NodeComponent> clone() const override;
};

} }
