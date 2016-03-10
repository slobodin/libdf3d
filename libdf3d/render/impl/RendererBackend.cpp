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
