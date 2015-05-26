#pragma once

#include "NodeComponent.h"
#include <render/RenderOperation.h>
#include <render/RenderPass.h>

namespace df3d { namespace components {

class DF3D_DLL Sprite2DComponent : public NodeComponent
{
    render::RenderOperation2D m_op;

    virtual void onDraw(render::RenderQueue *ops) override;
    virtual void onAttached() override;

public:
    Sprite2DComponent(const char *pathToTexture);
    ~Sprite2DComponent();

    void setSize(const glm::vec2 &v);
    void setWidth(float w);
    void setHeight(float h);

    glm::vec2 getSize();
    float getWidth();
    float getHeight();

    float getTextureWidth() const;
    float getTextureHeight() const;

    void setBlendMode(render::RenderPass::BlendingMode bm);

    // TODO:
    // Set anchor
    // Set width & height

    virtual shared_ptr<NodeComponent> clone() const override;
};

} }
