#pragma once

#include "NodeComponent.h"
#include <spark/SPARK.h>

FWD_MODULE_CLASS(render, RenderPass)
FWD_MODULE_CLASS(render, VertexBuffer)
FWD_MODULE_CLASS(render, RenderOperation)
FWD_MODULE_CLASS(render, RenderManager)

namespace df3d { namespace components {

class DF3D_DLL ParticleSystemComponent : public NodeComponent
{
    SPK_IMPLEMENT_SYSTEM_WRAPPER

private:
    bool m_paused = false;
    float m_systemLifeTime = -1.0f;
    float m_systemAge = 0.0f;

    void onUpdate(float dt) override;
    void onEvent(ComponentEvent ev) override;
    void onDraw(render::RenderQueue *ops) override;

public:
    ParticleSystemComponent();
    ~ParticleSystemComponent();

    size_t getParticlesCount() const;

    void stop();
    void pause(bool paused) { m_paused = paused; }
    void setSystemLifeTime(float lifeTime) { m_systemLifeTime = lifeTime; }

    shared_ptr<NodeComponent> clone() const override;
};

} }