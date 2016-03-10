#include "RenderPass.h"

#include <libdf3d/base/EngineController.h>
#include <libdf3d/resources/ResourceManager.h>
#include <libdf3d/resources/ResourceFactory.h>

namespace df3d {

RenderPassParam::RenderPassParam()
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

PassParamHandle RenderPass::getPassParamHandle(const std::string &name) const
{
    // TODO_render
    return 0;
}

RenderPassParam* RenderPass::getPassParam(PassParamHandle idx)
{
    // TODO_render
    return &m_params.at(42);
}

RenderPassParam* RenderPass::getPassParam(const std::string &name)
{
    // TODO_render:
    return nullptr;
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

void RenderPass::updateProgramParams()
{
    // TODO_render
    /*
    // Update shared uniforms.
    size_t uniCount = program->getSharedUniformsCount();

    for (size_t i = 0; i < uniCount; i++)
        m_programState->updateSharedUniform(program->getSharedUniform(i));

    // Update custom uniforms.
    auto &passParams = pass->getPassParams();
    uniCount = passParams.size();

    for (size_t i = 0; i < uniCount; i++)
        passParams[i].updateTo(program);

    // Update samplers.
    auto &samplers = pass->getSamplers();
    int textureUnit = 0;
    for (size_t i = 0; i < samplers.size(); i++)
    {
        shared_ptr<Texture> texture = samplers[i].texture;
        if (!texture)
            texture = m_whiteTexture;

        glActiveTexture(GL_TEXTURE0 + textureUnit);

        auto bound = texture->bind();
        if (!bound)
        {
            texture = m_whiteTexture;
            texture->bind();
        }

        samplers[i].update(program, textureUnit);

        textureUnit++;
    }
    */
}

}
