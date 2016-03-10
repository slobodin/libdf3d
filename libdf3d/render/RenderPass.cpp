#include "RenderPass.h"

#include <libdf3d/base/EngineController.h>
#include <libdf3d/resources/ResourceManager.h>
#include <libdf3d/resources/ResourceFactory.h>

namespace df3d {

RenderPassParam::RenderPassParam(const std::string &name)
    : m_name(name)
{

}

RenderPassParam::~RenderPassParam()
{

}

void RenderPassParam::setValue(float val)
{
    // TODO_render
}

void RenderPassParam::setValue(const glm::vec4 &val)
{
    // TODO_render
}

void RenderPassParam::setValue(shared_ptr<Texture> texture)
{
    // TODO_render
}

shared_ptr<Texture> RenderPassParam::getTexture()
{
    // TODO_render
    return nullptr;
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
        glog << "Failed to set empty gpu program to a render pass" << logwarn;
        return;
    }

    if (m_gpuProgram)
    {
        glog << "Can't set GPU program twice!!!! Should fix that" << logwarn;
        assert(false);
        return;
    }

    m_gpuProgram = newProgram;
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
    assert(idx >= 0 && idx < m_params.size());
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
