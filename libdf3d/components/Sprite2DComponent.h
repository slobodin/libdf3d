#pragma once

#include "NodeComponent.h"
#include <render/RenderOperation.h>

namespace df3d { namespace components {

class DF3D_DLL Sprite2DComponent : public NodeComponent
{
    render::RenderOperation m_op;

    virtual void onDraw(render::RenderQueue *ops) override;

public:
    Sprite2DComponent(const char *pathToTexture);
    ~Sprite2DComponent();

    virtual shared_ptr<NodeComponent> clone() const override;
};

} }
