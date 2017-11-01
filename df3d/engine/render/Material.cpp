#include "Material.h"

#include <df3d/engine/render/IRenderBackend.h>
#include <df3d/engine/resources/GpuProgramResource.h>

namespace df3d {

ValuePassParam::ValuePassParam()
{
    memset(&m_value, 0, sizeof(m_value));
}

ValuePassParam::~ValuePassParam()
{

}

void ValuePassParam::setValue(int val)
{
    m_value.intVal = val;
}

void ValuePassParam::setValue(float val)
{
    m_value.floatVal = val;
}

void ValuePassParam::setValue(const glm::vec4 &val)
{
    m_value.vec4Val[0] = val.x;
    m_value.vec4Val[1] = val.y;
    m_value.vec4Val[2] = val.z;
    m_value.vec4Val[3] = val.w;
}

void ValuePassParam::updateToProgram(IRenderBackend &backend, const GpuProgramResource &program, Id name)
{
    if (!m_handle.isValid())
    {
        auto handleFound = program.customUniforms.find(name);
        if (handleFound == program.customUniforms.end())
        {
            DFLOG_WARN("Failed to lookup uniform %s in a shader", name.toString().c_str());
            DF3D_ASSERT(false);
            return;
        }

        m_handle = handleFound->second;
    }

    backend.setUniformValue(program.handle, m_handle, &m_value);
}

void RenderPass::bindCustomPassParams(IRenderBackend &backend)
{
    // Samplers.
    int textureUnit = 0;
    for (auto &kv : m_samplers)
    {
        auto &sampler = kv.second;
        backend.bindTexture(sampler.texture, textureUnit);
        sampler.uniform.setValue(textureUnit);
        sampler.uniform.updateToProgram(backend, *program, kv.first);

        textureUnit++;
    }

    // Custom uniforms.
    for (auto &passParam : m_shaderParams)
        passParam.second.updateToProgram(backend, *program, passParam.first);
}

void RenderPass::setParam(Id name, TextureHandle texture)
{
    m_samplers[name].texture = texture;
}

void RenderPass::setParam(Id name, int value)
{
    m_shaderParams[name].setValue(value);
}

void RenderPass::setParam(Id name, float value)
{
    m_shaderParams[name].setValue(value);
}

void RenderPass::setParam(Id name, const glm::vec4 &value)
{
    m_shaderParams[name].setValue(value);
}

glm::vec4 RenderPass::getParamVec4(Id name)
{
    return m_shaderParams[name].getAsVec4();
}

void Material::addTechnique(const Technique &technique)
{
    auto found = std::find_if(m_techniques.begin(), m_techniques.end(), [&technique](const Technique &other) {
        return other.name == technique.name;
    });

    if (found == m_techniques.end())
        m_techniques.push_back(technique);
    else
        DFLOG_WARN("Duplicate technique!");
}

Technique* Material::getCurrentTechnique()
{
    if (m_currentTechIdx < 0 || m_currentTechIdx >= m_techniques.size())
        return nullptr;
    return &m_techniques[m_currentTechIdx];
}

const Technique* Material::getCurrentTechnique() const
{
    if (m_currentTechIdx < 0 || m_currentTechIdx >= m_techniques.size())
        return nullptr;
    return &m_techniques[m_currentTechIdx];
}

void Material::setCurrentTechnique(Id name)
{
    auto found = std::find_if(m_techniques.begin(), m_techniques.end(), [name](const Technique &other) {
        return other.name == name;
    });

    if (found != m_techniques.end())
        m_currentTechIdx = std::distance(m_techniques.begin(), found);
    else
        DFLOG_WARN("Failed to set technique");
}

Id PREFERRED_TECHNIQUE;

}
