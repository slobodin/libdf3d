#pragma once

#include "NodeComponent.h"
#include <SPARK.h>

namespace df3d {

class DF3D_DLL ParticleSystemComponent : public NodeComponent
{
    SPK_IMPLEMENT_SYSTEM_WRAPPER

private:
    bool m_paused = false;
    bool m_worldTransformed = true;
    float m_systemLifeTime = -1.0f;
    float m_systemAge = 0.0f;

    void updateCameraPosition();

    void onUpdate(float dt) override;
    void onDraw(RenderQueue *ops) override;

public:
    ParticleSystemComponent();
    ~ParticleSystemComponent();

    size_t getParticlesCount() const;

    void stop();
    void pause(bool paused) { m_paused = paused; }
    void setSystemLifeTime(float lifeTime) { m_systemLifeTime = lifeTime; }
    void setWorldTransformed(bool worldTransformed) { m_worldTransformed = worldTransformed; }

    float getSystemLifeTime() const { return m_systemLifeTime; }

    shared_ptr<NodeComponent> clone() const override;
};

}
