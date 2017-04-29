#pragma once

#include <df3d/engine/render/RenderCommon.h>

namespace df3d {

class IRenderBackend;
struct GpuProgramResource;

class ValuePassParam
{
    UniformHandle m_handle;

    union
    {
        int intVal;
        float floatVal;
        float vec4Val[4];
    } m_value;

public:
    ValuePassParam();
    ~ValuePassParam();

    void setValue(int val);
    void setValue(float val);
    void setValue(const glm::vec4 &val);

    glm::vec4 getAsVec4() const { return glm::vec4(m_value.vec4Val[0], m_value.vec4Val[1], m_value.vec4Val[2], m_value.vec4Val[3]); }

    void updateToProgram(IRenderBackend &backend, const GpuProgramResource &program, Id name);
};

struct SamplerPassParam
{
    ValuePassParam uniform;
    TextureHandle texture;
};

class RenderPass
{
    std::unordered_map<Id, SamplerPassParam> m_samplers;
    std::unordered_map<Id, ValuePassParam> m_shaderParams;

public:
    const GpuProgramResource *program = nullptr;
    bool depthTest = true;
    bool depthWrite = true;
    bool isTransparent = false;
    bool lightingEnabled = false;
    FaceCullMode faceCullMode = FaceCullMode::BACK;
    BlendingMode blendMode = BlendingMode::NONE;

    void bindCustomPassParams(IRenderBackend &backend);
    void setParam(Id name, TextureHandle texture);
    void setParam(Id name, int value);
    void setParam(Id name, float value);
    void setParam(Id name, const glm::vec4 &value);

    glm::vec4 getParamVec4(Id name);
};

struct Technique
{
    std::vector<RenderPass> passes;
    Id name;
    Technique(Id name) : name(name) { }
};

class Material
{
    std::string m_name;
    std::vector<Technique> m_techniques;
    int m_currentTechIdx = -1;

public:
    Material() = default;
    ~Material() = default;

    void addTechnique(const Technique &technique);

    Technique* getCurrentTechnique();
    const Technique* getCurrentTechnique() const;
    void setCurrentTechnique(Id name);

    void setName(const std::string &name) { m_name = name; }
    Id getId() const { return Id(m_name.c_str()); }
    const std::string& getName() const { return m_name; }
};

// FIXME: temp workaround
extern Id PREFERRED_TECHNIQUE;

}
