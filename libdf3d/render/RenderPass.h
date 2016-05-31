#pragma once

#include <libdf3d/render/RenderCommon.h>

namespace df3d {

class Texture;
class GpuProgram;
class IRenderBackend;

class DF3D_DLL RenderPassParam
{
    std::string m_name;
    UniformDescriptor m_descr;

#ifdef _DEBUG
    bool m_bindingFailed = false;
#endif

    union
    {
        int intVal;
        float floatVal;
        float vec4Val[4];
    } m_value;

    shared_ptr<Texture> m_texture;
    bool m_hasTexture = false;

public:
    RenderPassParam(const std::string &name);
    ~RenderPassParam();

    const std::string& getName() const { return m_name; }

    void setValue(int val);
    void setValue(float val);
    void setValue(const glm::vec4 &val);
    void setValue(shared_ptr<Texture> texture);

    shared_ptr<Texture> getTexture();
    bool hasTexture() { return m_hasTexture; }

    void updateToProgram(IRenderBackend &backend, GpuProgram &program);
};

using PassParamHandle = size_t;
static const PassParamHandle InvalidPassParamHandle = std::numeric_limits<size_t>::max();

class DF3D_DLL RenderPass
{
    std::string m_name;

    shared_ptr<GpuProgram> m_gpuProgram;
    std::vector<RenderPassParam> m_params;

    FaceCullMode m_faceCullMode = FaceCullMode::BACK;
    BlendingMode m_blendMode = BlendingMode::NONE;

    bool m_isTransparent = false;
    bool m_lightingEnabled = false;
    bool m_depthTest = true;
    bool m_depthWrite = true;

    // FIXME: move df3d to pbr and remove this shit.
    glm::vec4 m_ambientColor = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);

public:
    RenderPass(const std::string &name = "");
    ~RenderPass();

    void setGpuProgram(shared_ptr<GpuProgram> newProgram);
    shared_ptr<GpuProgram> getGpuProgram() const;

    PassParamHandle getPassParamHandle(const std::string &name);
    // NOTE: do not cache return value as vector can be reallocated. Use PassParamHandle instead.
    // TODO: refactor this.
    RenderPassParam* getPassParam(PassParamHandle idx);
    RenderPassParam* getPassParam(const std::string &name);
    std::vector<RenderPassParam>& getPassParams() { return m_params; }

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

    const glm::vec4& getAmbientColor() const { return m_ambientColor; }
    void setAmbientColor(const glm::vec4 &color) { m_ambientColor = color; }
};

}
