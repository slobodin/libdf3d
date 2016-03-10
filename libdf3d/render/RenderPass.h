#pragma once

#include <libdf3d/render/RenderCommon.h>

namespace df3d {

class Texture;
class GpuProgram;

class DF3D_DLL RenderPassParam
{
public:
    RenderPassParam();
    // TODO_render: copy ctor
    ~RenderPassParam();

    void setValue(float val);
    void setValue(const glm::vec4 &val);
    void setValue(shared_ptr<Texture> texture);

    shared_ptr<Texture> getTexture();
};

using PassParamHandle = size_t;

class DF3D_DLL RenderPass
{
    std::string m_name;

    shared_ptr<GpuProgram> m_gpuProgram;
    std::vector<RenderPassParam> m_params;

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
    ~RenderPass();

    void setGpuProgram(shared_ptr<GpuProgram> newProgram);
    shared_ptr<GpuProgram> getGpuProgram() const;

    PassParamHandle getPassParamHandle(const std::string &name) const;
    // NOTE: do not cache return value.
    RenderPassParam* getPassParam(PassParamHandle idx);
    RenderPassParam* getPassParam(const std::string &name);

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

    void updateProgramParams();
};

}
