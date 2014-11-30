#include "df3d_pch.h"
#include "GpuProgram.h"

#include <base/Controller.h>
#include <resources/ResourceManager.h>
#include "Shader.h"
#include "RenderPass.h"
#include "OpenGLCommon.h"
#include "Vertex.h"

namespace df3d { namespace render {

void gpuProgramLog(unsigned int program)
{
    int infologLen = 0;
    char *infoLog = nullptr;

    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infologLen);
    infoLog = new char[infologLen + 1];

    glGetProgramInfoLog(program, infologLen, nullptr, infoLog);
    base::glog << "GPU program info log:" << infoLog << base::logmess;

    delete [] infoLog;
}

bool isSampler(GLenum type)
{
#if defined(__WINDOWS__)
    return type == GL_SAMPLER_1D || type == GL_SAMPLER_2D || type == GL_SAMPLER_3D || type == GL_SAMPLER_CUBE;
#elif defined(__ANDROID__)
    return type == GL_SAMPLER_2D || type == GL_SAMPLER_CUBE;
#else
#error "Unsupported plarform."
#endif
}

void GpuProgram::compileShaders()
{
    if (m_shadersCompiled)
        return;

    if (m_programDescriptor == 0)
        m_programDescriptor = glCreateProgram();

    for (auto shader : m_shaders)
    {
        if (!shader || !shader->compile())
        {
            base::glog << "Failed to compile shaders in" << m_guid << base::logwarn;
            return;
        }
    }

    m_shadersCompiled = true;
}

void GpuProgram::attachShaders()
{
    if (m_shadersAttached || m_programDescriptor == 0)
        return;

    for (auto shader : m_shaders)
        glAttachShader(m_programDescriptor, shader->getDescriptor());

//#if defined(__ANDROID__)
    glBindAttribLocation(m_programDescriptor, VertexComponent::POSITION, "vertex");
    glBindAttribLocation(m_programDescriptor, VertexComponent::NORMAL, "normal");
    glBindAttribLocation(m_programDescriptor, VertexComponent::TEXTURE_COORDS, "txCoord");
    glBindAttribLocation(m_programDescriptor, VertexComponent::COLOR, "vertexColor");
    glBindAttribLocation(m_programDescriptor, VertexComponent::TANGENT, "tangent");
    glBindAttribLocation(m_programDescriptor, VertexComponent::BITANGENT, "bitangent");
//#endif

    glLinkProgram(m_programDescriptor);

    int linkOk;
    glGetProgramiv(m_programDescriptor, GL_LINK_STATUS, &linkOk);
    if (linkOk == GL_FALSE)
    {
        gpuProgramLog(m_programDescriptor);
        return;
    }

    requestUniforms();

    m_shadersAttached = true;
}

void GpuProgram::requestUniforms()
{
    int total = -1;
    glGetProgramiv(m_programDescriptor, GL_ACTIVE_UNIFORMS, &total);

    for (int i = 0; i < total; i++)
    {
        GLenum type = GL_ZERO;
        int nameLength = -1, uniformVarSize = -1;
        char name[100];

        glGetActiveUniform(m_programDescriptor, i, sizeof(name) - 1, &nameLength, &uniformVarSize, &type, name);
        name[nameLength] = 0;

        GpuProgramUniform uni(name);
        uni.m_location = glGetUniformLocation(m_programDescriptor, name);
        uni.m_glType = type;
        uni.m_isSampler = isSampler(type);

        if (uni.isShared())
            m_sharedUniforms.push_back(uni);
        else
            m_customUniforms.push_back(uni);
    }
}

GpuProgram::GpuProgram()
{
}

GpuProgram::~GpuProgram()
{
    if (m_programDescriptor == 0)
        return;

    unbind();

    for (auto shader : m_shaders)
    {
        if (shader->getDescriptor() != 0)
            glDetachShader(m_programDescriptor, shader->getDescriptor());
    }

    glDeleteProgram(m_programDescriptor);
}

void GpuProgram::attachShader(shared_ptr<Shader> shader)
{
    m_shaders.push_back(shader);
    m_shadersCompiled = false;
    m_shadersAttached = false;
}

void GpuProgram::detachShader(shared_ptr<Shader> shader)
{
    auto erasepos = std::find_if(m_shaders.cbegin(), m_shaders.cend(), 
        [=](const shared_ptr<Shader> val) -> bool { return shader->getDescriptor() == val->getDescriptor(); });

    if (erasepos == m_shaders.cend())
        return;

    m_shaders.erase(erasepos);
    glDetachShader(m_programDescriptor, shader->getDescriptor());

    m_shadersCompiled = false;
    m_shadersAttached = false;
}

void GpuProgram::bind()
{
    if (!valid() || m_shaders.empty())
        return;

    compileShaders();
    if (!m_shadersCompiled)
        return;

    attachShaders();
    if (!m_shadersAttached)
        return;

    glUseProgram(m_programDescriptor);
}

void GpuProgram::unbind()
{
    if (m_programDescriptor == 0)
        return;
    glUseProgram(0);
}

bool GpuProgram::init()
{
    return true;
}

GpuProgramUniform *GpuProgram::getCustomUniform(const std::string &name)
{
    for (size_t i = 0; i < m_customUniforms.size(); i++)
    {
        if (m_customUniforms[i].getName() == name)
            return &m_customUniforms[i];
    }

    return nullptr;
}

shared_ptr<GpuProgram> GpuProgram::create(const char *vshader, const char *fshader)
{
    // FIXME:
    // Use full path.
    std::string gpuProgramId = std::string(vshader) + ";" + fshader;
    if (g_resourceManager->isResourceExists(gpuProgramId.c_str()))
        return g_resourceManager->getResource<GpuProgram>(gpuProgramId.c_str());

    auto program = make_shared<GpuProgram>();
    auto vertexShader = Shader::createFromFile(vshader);
    auto fragmentShader = Shader::createFromFile(fshader);

    if (!vertexShader || !fragmentShader)
    {
        base::glog << "Can not create gpu program. Either vertex or fragment shader is invalid" << base::logwarn;
        return nullptr;
    }

    program->attachShader(vertexShader);
    program->attachShader(fragmentShader);

    program->setGUID(gpuProgramId);
    program->setInitialized(program->init());
    // Store in resource manager.
    g_resourceManager->loadResource(program);

    return program;
}

shared_ptr<GpuProgram> GpuProgram::createSimpleLightingGpuProgram()
{
    return g_resourceManager->getResource<GpuProgram>(SIMPLE_LIGHTING_PROGRAM_EMBED_PATH);
}

shared_ptr<GpuProgram> GpuProgram::createFFP2DGpuProgram()
{
    return g_resourceManager->getResource<GpuProgram>(FFP2D_PROGRAM_EMBED_PATH);
}

} }
