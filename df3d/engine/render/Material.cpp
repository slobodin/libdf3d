#include "Material.h"

#include <df3d/engine/render/IRenderBackend.h>
#include <df3d/engine/resources/GpuProgramResource.h>

namespace df3d {

ValuePassParam& RenderPass::getOrCreate(Id name)
{
    for (size_t i = 0; i < m_paramNames.size(); i++)
    {
        if (m_paramNames[i] == name)
            return m_params[i];
    }

    m_paramNames.push_back(name);
    m_params.push_back({});

    return m_params.back();
}

void RenderPass::setDepthWrite(bool value)
{
    state &= ~RENDER_STATE_DEPTH_WRITE_MASK;
    if (value)
        state |= RENDER_STATE_DEPTH_WRITE;
}

void RenderPass::setDepthTest(bool value)
{
    state &= ~RENDER_STATE_DEPTH_MASK;
    if (value)
        state |= RENDER_STATE_DEPTH_LEQUAL;
}

void RenderPass::setBlending(Blending value)
{
    state &= ~RENDER_STATE_BLENDING_MASK;
    switch (value)
    {
    case Blending::ADD:
        state |= BLENDING_ADD;
        break;
    case Blending::ALPHA:
        state |= BLENDING_ALPHA;
        break;
    case Blending::ADDALPHA:
        state |= BLENDING_ADDALPHA;
        break;
    default:
        break;
    }
}

void RenderPass::setBackFaceCullingEnabled(bool enabled)
{
    state &= ~RENDER_STATE_FACE_CULL_MASK;
    if (enabled)
        state |= RENDER_STATE_FRONT_FACE_CCW;
}

void RenderPass::bindCustomPassParams(IRenderBackend &backend)
{
    int textureUnitCounter = 0;
    for (size_t i = 0; i < m_paramNames.size(); i++)
    {
        auto &param = m_params[i];
        Id paramName = m_paramNames[i];

        // Acquire uniform handle from the shader.
        if (!param.uniformHandle.isValid())
        {
            auto handleFound = program->customUniforms.find(paramName);
            if (handleFound == program->customUniforms.end())
            {
                DFLOG_WARN("Failed to lookup uniform %s in a shader", paramName.toString().c_str());
                DF3D_ASSERT(false);
                continue;
            }

            param.uniformHandle = handleFound->second;
        }

        if (param.type == ValuePassParam::TEXTURE)
        {
            backend.bindTexture(program->handle, TextureHandle(param.value.textureHandle), param.uniformHandle, textureUnitCounter++);
        }
        else
        {
            DF3D_ASSERT(param.type != ValuePassParam::COUNT);
            backend.setUniformValue(program->handle, param.uniformHandle, &param.value);
        }
    }
}

void RenderPass::setParam(Id name, TextureHandle texture)
{
    auto &param = getOrCreate(name);
    param.type = ValuePassParam::TEXTURE;
    param.value.textureHandle = texture.getID();
}

void RenderPass::setParam(Id name, float value)
{
    auto &param = getOrCreate(name);
    param.type = ValuePassParam::FLOAT;
    param.value.floatVal = value;
}

void RenderPass::setParam(Id name, const glm::vec4 &value)
{
    auto &param = getOrCreate(name);
    param.type = ValuePassParam::VEC4;
    param.value.vec4Val[0] = value.x;
    param.value.vec4Val[1] = value.y;
    param.value.vec4Val[2] = value.z;
    param.value.vec4Val[3] = value.w;
}

glm::vec4 RenderPass::paramAsVec4(Id name)
{
    auto &param = getOrCreate(name);
    DF3D_ASSERT(param.type == ValuePassParam::VEC4);
    return { param.value.vec4Val[0], param.value.vec4Val[1], param.value.vec4Val[2], param.value.vec4Val[3] };
}

float RenderPass::paramAsFloat(Id name)
{
    auto &param = getOrCreate(name);
    DF3D_ASSERT(param.type == ValuePassParam::FLOAT);
    return param.value.floatVal;
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

}
