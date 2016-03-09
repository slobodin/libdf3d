#include "RendererBackend.h"

#include <libdf3d/base/EngineController.h>
#include <libdf3d/base/FrameStats.h>
#include <libdf3d/resources/ResourceManager.h>
#include <libdf3d/resources/ResourceFactory.h>
#include <libdf3d/3d/Light.h>
#include "OpenGLCommon.h"
#include "GpuProgramState.h"
#include "RenderPass.h"
#include "Shader.h"
#include "GpuProgram.h"
#include "VertexIndexBuffer.h"
#include "GpuProgramUniform.h"
#include "Texture2D.h"
#include "Viewport.h"

namespace df3d {

void RendererBackend::setBlendMode(RenderPass::BlendingMode bm)
{
    if (m_blendModeOverriden)
        return;

    switch (bm)
    {
    case RenderPass::BlendingMode::NONE:
        glDisable(GL_BLEND);
        break;
    case RenderPass::BlendingMode::ADDALPHA:
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        break;
    case RenderPass::BlendingMode::ALPHA:
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        break;
    case RenderPass::BlendingMode::ADD:
        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ONE);
        break;
    default:
        break;
    }
}

void RendererBackend::setCullFace(RenderPass::FaceCullMode cm)
{
    if (m_cullFaceOverriden)
        return;

    switch (cm)
    {
    case RenderPass::FaceCullMode::NONE:
        glDisable(GL_CULL_FACE);
        break;
    case RenderPass::FaceCullMode::FRONT:
        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT);
        break;
    case RenderPass::FaceCullMode::BACK:
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        break;
    case RenderPass::FaceCullMode::FRONT_AND_BACK:
        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT_AND_BACK);
        break;
    default:
        break;
    }
}

void RendererBackend::updateProgramUniformValues(GpuProgram *program, RenderPass *pass)
{
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
}


RendererBackend::RendererBackend()
    : m_programState(new GpuProgramState())
{

}

RendererBackend::~RendererBackend()
{
}

void RendererBackend::loadResources()
{

}

void RendererBackend::setViewport(const Viewport &viewport)
{

}

void RendererBackend::enableDepthTest(bool enable)
{
    if (m_depthTestOverriden)
        return;

    if (enable)
        glEnable(GL_DEPTH_TEST);
    else
        glDisable(GL_DEPTH_TEST);
}

void RendererBackend::enableDepthWrite(bool enable)
{
    if (m_depthWriteOverriden)
        return;

    glDepthMask(enable);
}

void RendererBackend::enableFog(float density, const glm::vec3 &fogColor)
{
    m_programState->m_fogDensity = density;
    m_programState->m_fogColor = fogColor;
}

void RendererBackend::enableBlendModeOverride(RenderPass::BlendingMode bm)
{
    m_blendModeOverriden = false;
    setBlendMode(bm);
    m_blendModeOverriden = true;
}

void RendererBackend::enableFrontFaceOverride(RenderPass::WindingOrder wo)
{
    m_frontFaceOverriden = false;
    setFrontFace(wo);
    m_frontFaceOverriden = true;
}

void RendererBackend::enableCullFaceOverride(RenderPass::FaceCullMode cm)
{
    m_cullFaceOverriden = false;
    setCullFace(cm);
    m_cullFaceOverriden = true;
}

void RendererBackend::enablePolygonDrawModeOverride(RenderPass::PolygonMode pm)
{
    m_polygonDrawModeOverriden = false;
    setPolygonDrawMode(pm);
    m_polygonDrawModeOverriden = true;
}

void RendererBackend::enableDepthTestOverride(bool enable)
{
    m_depthTestOverriden = false;
    enableDepthTest(enable);
    m_depthTestOverriden = true;
}

void RendererBackend::enableDepthWriteOverride(bool enable)
{
    m_depthWriteOverriden = false;
    enableDepthWrite(enable);
    m_depthWriteOverriden = true;
}

void RendererBackend::setWorldMatrix(const glm::mat4 &worldm)
{
    m_programState->setWorldMatrix(worldm);
}

void RendererBackend::setCameraMatrix(const glm::mat4 &viewm)
{
    m_programState->setViewMatrix(viewm);
}

void RendererBackend::setProjectionMatrix(const glm::mat4 &projm)
{
    m_programState->setProjectionMatrix(projm);
}

void RendererBackend::setAmbientLight(const glm::vec3 &ambient)
{
    m_programState->m_globalAmbient = glm::vec4(ambient, 1.0f);
}

void RendererBackend::setAmbientLight(float ra, float ga, float ba)
{
    setAmbientLight(glm::vec3(ra, ga, ba));
}

void RendererBackend::setLight(const Light &light)
{
    auto &glslLight = m_programState->m_currentLight;

    // Update light params.
    glslLight.diffuseParam = light.getDiffuseColor();
    glslLight.specularParam = light.getSpecularColor();
    glslLight.k0Param = light.getConstantAttenuation();
    glslLight.k1Param = light.getLinearAttenuation();
    glslLight.k2Param = light.getQuadraticAttenuation();

    // Since we calculate lighting in the view space we should translate position and direction.
    if (light.getType() == Light::Type::DIRECTIONAL)
    {
        auto dir = light.getDirection();
        glslLight.positionParam = m_programState->getViewMatrix() * glm::vec4(dir, 0.0f);
    }
    else if (light.getType() == Light::Type::POINT)
    {
        auto pos = light.getPosition();
        glslLight.positionParam = m_programState->getViewMatrix() * glm::vec4(pos, 1.0f);
    }
}

void RendererBackend::bindPass(RenderPass *pass)
{
    if (!pass)
        return;

    // TODO: check state changes on CPU side.
    // This may work incorrect when binding the same pass, but with params changed!
    /*
    if (pass == m_programState->m_currentPass)
    {
        updateProgramUniformValues(m_programState->m_currentShader, m_programState->m_currentPass);
        return;
    }
    */

    auto glprogram = pass->getGpuProgram();
    if (!glprogram)
    {
        glog << "Failed to bind pass. No GPU program" << logwarn;
        return;
    }

    m_programState->m_currentPass = pass;

    // FIXME:
    // Cache state.
    enableDepthTest(pass->depthTestEnabled());
    enableDepthWrite(pass->depthWriteEnabled());
    setBlendMode(pass->getBlendingMode());
    setFrontFace(pass->getFrontFaceWinding());
    setCullFace(pass->getFaceCullMode());
    setPolygonDrawMode(pass->getPolygonDrawMode());

    // Bind GPU program.
    if (!m_programState->m_currentShader || m_programState->m_currentShader != glprogram.get())
    {
        m_programState->m_currentShader = glprogram.get();
        m_programState->m_currentShader->bind();
    }

    updateProgramUniformValues(m_programState->m_currentShader, m_programState->m_currentPass);

    printOpenGLError();
}

void RendererBackend::drawVertexBuffer(VertexBuffer *vb, IndexBuffer *ib, RenderOperation::Type type)
{
    if (!vb)
        return;

    // FIXME: figure out why crashes on NVIDIA
    //if (m_prevVB != vb)
    {
        vb->bind();
        m_prevVB = vb;
    }

    if (ib != nullptr)
    {
        ib->bind();
        glDrawElements(convertRopType(type), ib->getIndicesUsed(), GL_UNSIGNED_INT, nullptr);
    }
    else
    {
        glDrawArrays(convertRopType(type), 0, vb->getVerticesUsed());
    }

    svc().getFrameStats().addRenderOperation(vb, ib, type);

    printOpenGLError();
}

void RendererBackend::drawOperation(const RenderOperation &op)
{
    if (op.isEmpty())
        return;

    setWorldMatrix(op.worldTransform);
    bindPass(op.passProps);
    drawVertexBuffer(op.vertexData, op.indexData, op.type);
}

}
