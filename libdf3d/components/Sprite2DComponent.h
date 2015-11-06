#pragma once

#include "NodeComponent.h"
#include <render/RenderOperation.h>
#include <render/RenderPass.h>

namespace df3d {

class DF3D_DLL Sprite2DComponent : public NodeComponent
{
    RenderOperation2D m_op;

    glm::vec2 m_anchor = glm::vec2(0.5f, 0.5f);
    glm::vec2 m_textureOriginalSize;
    glm::vec2 m_screenPosition;

    ResourceGUID m_textureGuid;
    
    virtual void onDraw(RenderQueue *ops) override;

public:
    Sprite2DComponent(const std::string &pathToTexture);
    ~Sprite2DComponent();

    void setAnchorPoint(const glm::vec2 &pt);
    void setZIdx(float z);

    void setSize(const glm::vec2 &size);
    void setWidth(float w);
    void setHeight(float h);

    glm::vec2 getSize();
    float getWidth();
    float getHeight();

    glm::vec2 getScreenPosition() const;

    void useTexture(const std::string &pathToTexture);
    glm::vec2 getTextureSize() const;

    void setBlendMode(RenderPass::BlendingMode bm);
    void setDiffuseColor(const glm::vec4 &diffuseColor);

    virtual shared_ptr<NodeComponent> clone() const override;
};

}
