#pragma once

#include <df3d/lib/Id.h>
#include <df3d/engine/render/RenderCommon.h>

namespace df3d {

class IRenderBackend;
struct GpuProgramResource;

struct ValuePassParam
{
    enum Type
    {
        FLOAT,
        VEC4,
        TEXTURE,

        COUNT
    };

    union
    {
        HandleType textureHandle;
        float floatVal;
        float vec4Val[4];
    } value;

    UniformHandle uniformHandle;
    Type type = COUNT;

    ValuePassParam()
    {
        memset(&value, 0, sizeof(value));
    }
};

class RenderPass
{
    std::vector<Id> m_paramNames;
    std::vector<ValuePassParam> m_params;

    ValuePassParam& getOrCreate(Id name);

public:
    const GpuProgramResource *program = nullptr;
    uint64_t state = RENDER_STATE_DEPTH_LESS | RENDER_STATE_DEPTH_WRITE | RENDER_STATE_FRONT_FACE_CCW;

    // FIXME:
    RenderQueueBucket preferredBucket = RQ_BUCKET_COUNT;

    void setDepthWrite(bool value);
    void setDepthTest(bool value);
    void setBlending(Blending value);
    void setBackFaceCullingEnabled(bool enabled);

    void bindCustomPassParams(IRenderBackend &backend);
    void setParam(Id name, TextureHandle texture);
    void setParam(Id name, float value);
    void setParam(Id name, const glm::vec4 &value);

    glm::vec4 paramAsVec4(Id name);
    float paramAsFloat(Id name);
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

}
