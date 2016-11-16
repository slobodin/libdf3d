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

    void updateToProgram(IRenderBackend &backend, const GpuProgramResource &program, const std::string &name);
};

struct SamplerPassParam
{
    ValuePassParam uniform;
    TextureHandle texture;
};

class RenderPass
{
    std::unordered_map<std::string, SamplerPassParam> m_samplers;
    std::unordered_map<std::string, ValuePassParam> m_shaderParams;

public:
    const GpuProgramResource *program = nullptr;
    bool depthTest = true;
    bool depthWrite = true;
    bool isTransparent = false;
    bool lightingEnabled = false;
    FaceCullMode faceCullMode = FaceCullMode::BACK;
    BlendingMode blendMode = BlendingMode::NONE;

    void bindCustomPassParams(IRenderBackend &backend);
    void setParam(const std::string &name, TextureHandle texture);
    void setParam(const std::string &name, int value);
    void setParam(const std::string &name, float value);
    void setParam(const std::string &name, const glm::vec4 &value);
};

struct Technique
{
    std::vector<RenderPass> passes;
    std::string name;
    Technique(const std::string &name) : name(name) { }
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
    void setCurrentTechnique(const std::string &name);

    void setName(const std::string &name) { m_name = name; }
    const std::string& getName() const { return m_name; }
};

// FIXME: temp workaround
extern std::string PREFERRED_TECHNIQUE;

}
