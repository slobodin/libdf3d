#include "RenderPass.h"

#include "GpuProgram.h"
#include "IRenderBackend.h"
#include <libdf3d/utils/Utils.h>

namespace df3d {

RenderPassParam::RenderPassParam(const std::string &name)
    : m_name(name)
{
    memset(&m_value, 0, sizeof(m_value));
}

RenderPassParam::~RenderPassParam()
{

}

void RenderPassParam::setValue(int val)
{
    m_value.intVal = val;
}

void RenderPassParam::setValue(float val)
{
    m_value.floatVal = val;
}

void RenderPassParam::setValue(const glm::vec4 &val)
{
    m_value.vec4Val[0] = val.x;
    m_value.vec4Val[1] = val.y;
    m_value.vec4Val[2] = val.z;
    m_value.vec4Val[3] = val.w;
}

void RenderPassParam::setValue(shared_ptr<Texture> texture)
{
    m_hasTexture = true;
    m_texture = texture;
}

shared_ptr<Texture> RenderPassParam::getTexture()
{
    return m_texture;
}

void RenderPassParam::updateToProgram(IRenderBackend &backend, GpuProgram &program)
{
#ifdef _DEBUG
    if (m_bindingFailed)
        return;
#endif

    if (!m_descr.valid())
    {
        auto descr = program.getCustomUniform(m_name);
        if (!descr.valid())
        {
#ifdef _DEBUG
            DFLOG_WARN("Failed to lookup uniform %s in a shader", m_name.c_str());
            m_bindingFailed = true;
            return;
#endif
        }

        m_descr = descr;
    }

    if (m_descr.valid())
        backend.setUniformValue(m_descr, &m_value);
}

RenderPass::RenderPass(const std::string &name)
    : m_name(name)
{

}

RenderPass::~RenderPass()
{
}

void RenderPass::setGpuProgram(shared_ptr<GpuProgram> newProgram)
{
    if (!newProgram)
    {
        DFLOG_WARN("Failed to set empty gpu program to a render pass");
        return;
    }

    if (m_gpuProgram)
    {
        DFLOG_WARN("Can't set GPU program twice!!!! Should fix that");
        DF3D_ASSERT_MESS(false, "not implemented");
        return;
    }

    m_gpuProgram = newProgram;

#ifdef _DEBUG
    auto customUniforms = m_gpuProgram->getCustomUniformNames();
    std::set<std::string> paramsSet;
    for (const auto &passParam : m_params)
        paramsSet.insert(passParam.getName());

    for (const auto &uniformName : customUniforms)
    {
        if (!utils::contains_key(paramsSet, uniformName))
            DFLOG_WARN("A uniform %s doesn't have a value in render pass params", uniformName.c_str());
    }
#endif
}

shared_ptr<GpuProgram> RenderPass::getGpuProgram() const
{
    return m_gpuProgram;
}

PassParamHandle RenderPass::getPassParamHandle(const std::string &name)
{
    auto found = std::find_if(m_params.begin(), m_params.end(), [&name](const RenderPassParam &it) { return it.getName() == name; });
    if (found == m_params.end())
    {
        RenderPassParam newparam(name);
        m_params.push_back(newparam);
        return m_params.size() - 1;
    }

    return std::distance(m_params.begin(), found);
}

RenderPassParam* RenderPass::getPassParam(PassParamHandle idx)
{
    DF3D_ASSERT(idx >= 0 && idx < m_params.size());
    return &m_params[idx];
}

RenderPassParam* RenderPass::getPassParam(const std::string &name)
{
    return getPassParam(getPassParamHandle(name));
}

void RenderPass::setFaceCullMode(FaceCullMode mode)
{
    m_faceCullMode = mode;
}

void RenderPass::setBlendMode(BlendingMode mode)
{
    m_blendMode = mode;
    m_isTransparent = m_blendMode != BlendingMode::NONE;
}

void RenderPass::enableDepthTest(bool enable)
{
    m_depthTest = enable;
}

void RenderPass::enableDepthWrite(bool enable)
{
    m_depthWrite = enable;
}

void RenderPass::enableLighting(bool enable)
{
    m_lightingEnabled = enable;
}

}
