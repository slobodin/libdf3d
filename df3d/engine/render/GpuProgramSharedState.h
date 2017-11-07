#pragma once

#include "RenderCommon.h"

namespace df3d {

class Light;
class Viewport;
struct GpuProgramResource;
class IRenderBackend;

class IGPUProgramSharedState : NonCopyable
{
public:
    IGPUProgramSharedState() = default;
    virtual ~IGPUProgramSharedState() = default;

    virtual void initialize(IRenderBackend *backend) = 0;

    virtual void setWorldMatrix(const glm::mat4 &worldm) = 0;
    virtual void setViewMatrix(const glm::mat4 &viewm) = 0;
    virtual void setProjectionMatrix(const glm::mat4 &projm) = 0;
    virtual void setViewPort(const Viewport &viewport) = 0;
    virtual void setFog(float density, const glm::vec3 &color) = 0;
    virtual void setAmbientColor(const glm::vec3 &color) = 0;
    virtual void setLight(const Light &light, size_t idx) = 0;

    virtual void updateSharedUniforms(const GpuProgramResource &program) = 0;

    static unique_ptr<IGPUProgramSharedState> create(RenderBackendID backendID);
};

}
