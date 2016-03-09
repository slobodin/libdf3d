#pragma once

#include <libdf3d/render/RenderCommon.h>

namespace df3d {

class Texture;
class GpuProgram;
class GpuProgramUniform;

struct DF3D_DLL RenderPassParamFloat
{
    // TODO_render: mb remove name?
    std::string name;
    float value = 0.0f;
};

struct DF3D_DLL RenderPassParamVec4
{
    std::string name;
    glm::vec4 value = {};
};

struct RenderPassParamTexture
{
    std::string name;
    shared_ptr<Texture> texture;
};

class DF3D_DLL RenderPass
{
    std::string m_name;

    shared_ptr<GpuProgram> m_gpuProgram;
    std::vector<RenderPassParamFloat> m_floatParams;
    std::vector<RenderPassParamVec4> m_vec4Params;

    //! Face culling mode.
    FaceCullMode m_faceCullMode = FaceCullMode::BACK;
    //! Blending.
    BlendingMode m_blendMode = BlendingMode::NONE;

    bool m_isTransparent = false;
    bool m_lightingEnabled = false;
    bool m_depthTest = true;
    bool m_depthWrite = true;

public:
    //! Creates a pass with default parameters.
    RenderPass(const std::string &name = "");
    RenderPass(const RenderPass& other) = default;
    RenderPass& operator= (const RenderPass& other) = default;
    ~RenderPass();

    void setGpuProgram(shared_ptr<GpuProgram> newProgram);
    shared_ptr<GpuProgram> getGpuProgram() const;

    void setParam(const std::string &name, float value);
    void setParam(const std::string &name, const glm::vec4 &value);
    // TODO_render: do not want shared_ptr here. Do not want pass to hold the resource.
    void setParam(const std::string &name, shared_ptr<Texture> texture);

    shared_ptr<Texture> getTextureParam(const std::string &name) const;
    float getFloatParam(const std::string &name) const;
    const glm::vec4& getVec4Param(const std::string &name) const;

    void setFaceCullMode(FaceCullMode mode);
    void setBlendMode(BlendingMode mode);
    void enableDepthTest(bool enable);
    void enableDepthWrite(bool enable);
    void enableLighting(bool enable);

    bool isTransparent() const { return m_isTransparent; }
    bool isLit() const { return m_lightingEnabled; }
    bool isDepthTestEnabled() const { return m_depthTest; }
    bool isDepthWriteEnabled() const { return m_depthWrite; }
    const std::string& getName() const { return m_name; }
    FaceCullMode getFaceCullMode() const { return m_faceCullMode; }
    BlendingMode getBlendingMode() const { return m_blendMode; }
};

}
