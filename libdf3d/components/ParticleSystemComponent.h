#pragma once

#include "NodeComponent.h"
#include <utils/ParticleSystemComponentSerializer.h>

FWD_MODULE_CLASS(render, RenderPass)
FWD_MODULE_CLASS(render, VertexBuffer)
FWD_MODULE_CLASS(render, RenderOperation)
FWD_MODULE_CLASS(render, RenderManager)

namespace SPK { class System; }

namespace df3d { namespace components {

class DF3D_DLL ParticleSystemComponent : public NodeComponent
{
    // fk this sht
    friend void df3d::utils::serializers::load(components::ParticleSystemComponent *component, const char *vfxDefinitionFile);

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

    ParticleSystemComponent();

public:
    ParticleSystemComponent(const char *vfxDefinitionFile);
    ~ParticleSystemComponent();

    size_t getParticlesCount() const;

    void stop();
    void pause(bool paused) { m_paused = paused; }

    SPK::System *getSpk() { return m_system; }

    shared_ptr<NodeComponent> clone() const override;
};

} }