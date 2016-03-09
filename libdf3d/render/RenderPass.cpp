#include "RenderPass.h"

#include <libdf3d/base/EngineController.h>
#include <libdf3d/resources/ResourceManager.h>
#include <libdf3d/resources/ResourceFactory.h>

namespace df3d {

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

void RenderPass::setParam(const std::string &name, float value)
{
    // TODO_render:
    assert(false);
}

void RenderPass::setParam(const std::string &name, const glm::vec4 &value)
{
    // TODO_render:
    assert(false);
}

void RenderPass::setParam(const std::string &name, shared_ptr<Texture> texture)
{
    // TODO_render:
    assert(false);
}

shared_ptr<Texture> RenderPass::getTextureParam(const std::string &name) const
{
    // TODO_render:
    assert(false);
    return{};
}

float RenderPass::getFloatParam(const std::string &name) const
{
    // TODO_render:
    assert(false);
    return{};
}

const glm::vec4& RenderPass::getVec4Param(const std::string &name) const
{
    // TODO_render:
    assert(false);
    return{};
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
