#pragma once

#include "NodeComponent.h"

FWD_MODULE_CLASS(render, RenderPass)
FWD_MODULE_CLASS(render, VertexBuffer)
FWD_MODULE_CLASS(render, RenderOperation)
FWD_MODULE_CLASS(render, RenderManager)

namespace SPK { class System; }

namespace df3d { namespace components {

namespace serializers { class ParticleSystemComponentSerializer; }

class DF3D_DLL ParticleSystemComponent : public NodeComponent
{
    friend class df3d::components::serializers::ParticleSystemComponentSerializer;

    SPK::System *m_system = nullptr;
    bool m_paused = false;
    float m_systemLifeTime = -1.0f;
    float m_systemAge = 0.0f;

    // One render operation per group.
    std::vector<render::RenderOperation *> m_renderOps;

    void onUpdate(float dt) override;

    size_t getGroupCount();
    void prepareRenderOperations();
    const render::RenderOperation &getRenderOperation(size_t groupIdx);

    void onDraw(render::RenderQueue *ops) override;

public:
    ParticleSystemComponent();
    ~ParticleSystemComponent();

    size_t getParticlesCount() const;

    void stop();
    void pause(bool paused) { m_paused = paused; }

    SPK::System *getSpk() { return m_system; }

    shared_ptr<NodeComponent> clone() const override;
};

} }