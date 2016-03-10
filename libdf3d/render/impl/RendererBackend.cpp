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

void RendererBackend::enableFog(float density, const glm::vec3 &fogColor)
{
    m_programState->m_fogDensity = density;
    m_programState->m_fogColor = fogColor;
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
