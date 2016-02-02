#include "RendererBackend.h"

#include <libdf3d/base/EngineController.h>
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

void RendererBackend::createWhiteTexture()
{
    const auto w = 8;
    const auto h = 8;
    const auto pf = PixelFormat::RGBA;

    auto data = new unsigned char[w * h * 4];
    memset(data, 255, w * h * 4);

    TextureCreationParams params;
    params.setFiltering(TextureFiltering::NEAREST);
    params.setMipmapped(false);
    params.setWrapMode(TextureWrapMode::WRAP);
    params.setAnisotropyLevel(NO_ANISOTROPY);

    auto pb = make_unique<PixelBuffer>(w, h, data, pf);

    m_whiteTexture = svc().resourceManager().getFactory().createTexture(std::move(pb), params);
    m_whiteTexture->setResident(true);

    delete[] data;
}

void RendererBackend::loadResidentGpuPrograms()
{
    const std::string simple_lighting_vert =
#include "libdf3d/render/embed_glsl/simple_lighting_vert.h"
        ;
    const std::string simple_lighting_frag =
#include "libdf3d/render/embed_glsl/simple_lighting_frag.h"
        ;
    const std::string rtt_quad_vert =
#include "libdf3d/render/embed_glsl/rtt_quad_vert.h"
        ;
    const std::string rtt_quad_frag =
#include "libdf3d/render/embed_glsl/rtt_quad_frag.h"
        ;
    const std::string colored_vert =
#include "libdf3d/render/embed_glsl/colored_vert.h"
        ;
    const std::string colored_frag =
#include "libdf3d/render/embed_glsl/colored_frag.h"
        ;
    const std::string ambient_vert =
#include "libdf3d/render/embed_glsl/ambient_vert.h"
        ;
    const std::string ambient_frag =
#include "libdf3d/render/embed_glsl/ambient_frag.h"
        ;

    auto &factory = svc().resourceManager().getFactory();

    factory.createGpuProgram(SIMPLE_LIGHTING_PROGRAM_EMBED_PATH, simple_lighting_vert, simple_lighting_frag)->setResident(true);
    factory.createGpuProgram(RTT_QUAD_PROGRAM_EMBED_PATH, rtt_quad_vert, rtt_quad_frag)->setResident(true);
    factory.createGpuProgram(COLORED_PROGRAM_EMBED_PATH, colored_vert, colored_frag)->setResident(true);
    factory.createGpuProgram(AMBIENT_PASS_PROGRAM_EMBED_PATH, ambient_vert, ambient_frag)->setResident(true);
}

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

void RendererBackend::setFrontFace(RenderPass::WindingOrder wo)
{
    if (m_frontFaceOverriden)
        return;

    switch (wo)
    {
    case RenderPass::WindingOrder::CW:
        glFrontFace(GL_CW);
        break;
    case RenderPass::WindingOrder::CCW:
        glFrontFace(GL_CCW);
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

void RendererBackend::setPolygonDrawMode(RenderPass::PolygonMode pm)
{
    if (m_polygonDrawModeOverriden)
        return;

    // Doesn't work in OpenGL ES 2.x
#if defined(DF3D_DESKTOP)
    switch (pm)
    {
    case RenderPass::PolygonMode::FILL:
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        break;
    case RenderPass::PolygonMode::WIRE:
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        break;
    default:
        break;
    }
#endif
}

void RendererBackend::updateProgramUniformValues(GpuProgram *program, RenderPass *pass)
{
    // Update shader uniforms.
    size_t uniCount = program->getSharedUniformsCount();

    for (size_t i = 0; i < uniCount; i++)
        m_programState->updateSharedUniform(program->getSharedUniform(i));

    // Update custom uniforms.
    auto &passParams = pass->getPassParams();
    uniCount = passParams.size();

    for (size_t i = 0; i < uniCount; i++)
        passParams[i].updateTo(program);
}

void RendererBackend::updateTextureSamplers()
{
    // Bind textures to samplers.
    auto &samplers = m_programState->m_currentPass->getSamplers();
    size_t textureUnit = 0;
    for (size_t i = 0; i < samplers.size(); i++)
    {
        shared_ptr<Texture> texture = samplers[i].texture;
        if (!texture)
            texture = m_whiteTexture;

        auto bound = texture->bind(textureUnit);
        if (!bound)
        {
            texture = m_whiteTexture;
            texture->bind(textureUnit);
        }

        // FIXME:
        auto location = glGetUniformLocation(m_programState->m_currentShader->descriptor(), samplers[i].name.c_str());
        if (location != -1)
            glUniform1i(location, textureUnit++);
    }
}

RendererBackend::RendererBackend()
    : m_programState(new GpuProgramState())
{
#ifdef DF3D_DESKTOP
    // Init GLEW.
    glewExperimental = GL_TRUE;

    auto glewerr = glewInit();
    if (glewerr != GLEW_OK)
    {
        std::string errStr = "GLEW initialization failed: ";
        errStr += (const char *)glewGetErrorString(glewerr);
        throw std::runtime_error(errStr);
    }

    if (!glewIsSupported("GL_VERSION_2_1"))
        throw std::runtime_error("GL 2.1 unsupported");
#endif

    const char *ver = (const char *)glGetString(GL_VERSION);
    glog << "OpenGL version" << ver << logmess;

    const char *card = (const char *)glGetString(GL_RENDERER);
    const char *vendor = (const char *)glGetString(GL_VENDOR);
    glog << "Using" << card << vendor << logmess;

    const char *shaderVer = (const char *)glGetString(GL_SHADING_LANGUAGE_VERSION);
    glog << "Shaders version" << shaderVer << logmess;

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &m_maxTextureSize);
    // TODO:
    // Check extension supported.
    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &m_maxAnisotropyLevel);

    printOpenGLError();

    m_initialized = true;
}

RendererBackend::~RendererBackend()
{
}

void RendererBackend::loadResources()
{
    createWhiteTexture();
    loadResidentGpuPrograms();
}

void RendererBackend::setRenderStatsLocation(RenderStats *renderStats)
{
    m_renderStats = renderStats;
}

void RendererBackend::beginFrame()
{
    clearColorBuffer();
    clearDepthBuffer();
    clearStencilBuffer();
    enableDepthTest(true);
    enableDepthWrite(true);
    enableScissorTest(false);
    setBlendMode(RenderPass::BlendingMode::NONE);

    m_programState->onFrameBegin();

    // Clear previous frame GL error.
    printOpenGLError();
}

void RendererBackend::endFrame()
{
    m_programState->onFrameEnd();

    glFlush();
}

void RendererBackend::setViewport(const Viewport &viewport)
{
    glViewport(viewport.x(), viewport.y(), viewport.width(), viewport.height());

    m_programState->m_pixelSize = glm::vec2(1.0f / (float)viewport.width(), 1.0f / (float)viewport.height());
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

void RendererBackend::enableScissorTest(bool enable)
{
    if (enable)
        glEnable(GL_SCISSOR_TEST);
    else
        glDisable(GL_SCISSOR_TEST);
}

void RendererBackend::setScissorRegion(int x, int y, int width, int height)
{
    glScissor(x, y, width, height);
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

void RendererBackend::clearColorBuffer(const glm::vec4 &color)
{
    glClearColor(color.r, color.g, color.b, color.a);
    glClear(GL_COLOR_BUFFER_BIT);
}

void RendererBackend::clearDepthBuffer()
{
    glClear(GL_DEPTH_BUFFER_BIT);
}

void RendererBackend::clearStencilBuffer()
{
    glClear(GL_STENCIL_BUFFER_BIT);
}

float RendererBackend::readDepthBuffer(int x, int y)
{
    float z = 0.0f;
    glReadPixels(x, y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &z);
    return z;
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

    if (pass == m_programState->m_currentPass)
    {
        updateProgramUniformValues(m_programState->m_currentShader, m_programState->m_currentPass);
        updateTextureSamplers();
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

    auto glprogram = pass->getGpuProgram();
    // Sanity check.
    if (!glprogram)
        return;

    // Bind GPU program.
    if (!m_programState->m_currentShader || m_programState->m_currentShader != glprogram.get())
    {
        m_programState->m_currentShader = glprogram.get();
        m_programState->m_currentShader->bind();
    }

    updateProgramUniformValues(m_programState->m_currentShader, m_programState->m_currentPass);
    updateTextureSamplers();

    printOpenGLError();
}

void RendererBackend::drawVertexBuffer(VertexBuffer *vb, IndexBuffer *ib, RenderOperation::Type type)
{
    if (!vb)
        return;

    bool indexed = ib != nullptr;

    vb->bind();
    if (indexed)
        ib->bind();

    switch (type)
    {
    case RenderOperation::Type::LINES:
        if (indexed)
            glDrawElements(GL_LINES, ib->getIndicesUsed(), GL_UNSIGNED_INT, nullptr);
        else
            glDrawArrays(GL_LINES, 0, vb->getVerticesUsed());
        break;
    case RenderOperation::Type::TRIANGLES:
        if (indexed)
            glDrawElements(GL_TRIANGLES, ib->getIndicesUsed(), GL_UNSIGNED_INT, nullptr);
        else
            glDrawArrays(GL_TRIANGLES, 0, vb->getVerticesUsed());
        break;
    default:
        break;
    }

    //vb->unbind();
    //if (indexed)
    //    ib->unbind();

    if (m_renderStats)
    {
        m_renderStats->drawCalls++;

        // FIXME:
        if (type != RenderOperation::Type::LINES)
            m_renderStats->totalTriangles += indexed ? ib->getIndicesUsed() / 3 : vb->getVerticesUsed() / 3;
    }

    printOpenGLError();
}

int RendererBackend::getMaxTextureSize()
{
    if (!m_initialized)
    {
        glog << "Failed to get max texture size. RendererBackend is not initialized" << logwarn;
        return -1;
    }

    return m_maxTextureSize;
}

float RendererBackend::getMaxAnisotropy()
{
    if (!m_initialized)
    {
        glog << "Failed to get max anisotropy level. RendererBackend is not initialized" << logwarn;
        return 1.0f;
    }

    return m_maxAnisotropyLevel;
}

void RendererBackend::drawOperation(const RenderOperation &op)
{
    if (op.isEmpty())
        return;

    setWorldMatrix(op.worldTransform);
    bindPass(op.passProps.get());
    drawVertexBuffer(op.vertexData.get(), op.indexData.get(), op.type);
}

}
