#include "GpuProgram.h"

#include <base/EngineController.h>
#include <resources/ResourceManager.h>
#include "Shader.h"
#include "OpenGLCommon.h"
#include "Vertex.h"

namespace df3d {
namespace render {

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
#if defined(DF3D_DESKTOP)
    return type == GL_SAMPLER_1D || type == GL_SAMPLER_2D || type == GL_SAMPLER_3D || type == GL_SAMPLER_CUBE;
#else
    return type == GL_SAMPLER_2D || type == GL_SAMPLER_CUBE;
#endif
}

bool GpuProgram::compileShaders()
{
    assert(!m_programDescriptor);

    m_programDescriptor = glCreateProgram();

    for (auto shader : m_shaders)
    {
        if (!shader || !shader->compile())
        {
            base::glog << "Failed to compile shaders in" << getGUID() << base::logwarn;
            return false;
        }
    }

    return true;
}

bool GpuProgram::attachShaders()
{
    if (!m_programDescriptor)
        return false;

    for (auto shader : m_shaders)
        glAttachShader(m_programDescriptor, shader->getDescriptor());

    glBindAttribLocation(m_programDescriptor, VertexFormat::POSITION_3, "a_vertex3");
    glBindAttribLocation(m_programDescriptor, VertexFormat::NORMAL_3, "a_normal");
    glBindAttribLocation(m_programDescriptor, VertexFormat::TX_2, "a_txCoord");
    glBindAttribLocation(m_programDescriptor, VertexFormat::COLOR_4, "a_vertexColor");
    glBindAttribLocation(m_programDescriptor, VertexFormat::TANGENT_3, "a_tangent");
    glBindAttribLocation(m_programDescriptor, VertexFormat::BITANGENT_3, "a_bitangent");

    glLinkProgram(m_programDescriptor);

    int linkOk;
    glGetProgramiv(m_programDescriptor, GL_LINK_STATUS, &linkOk);
    if (linkOk == GL_FALSE)
    {
        base::glog << "GPU program linkage failed" << base::logwarn;
        gpuProgramLog(m_programDescriptor);
        return false;
    }

    requestUniforms();

    printOpenGLError();

    return true;
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

GpuProgram::GpuProgram(const std::vector<shared_ptr<Shader>> &shaders)
    : m_shaders(shaders)
{
    assert(!shaders.empty());

    if (compileShaders())
        m_initialized = attachShaders();
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

void GpuProgram::bind()
{
    if (!isInitialized())
        return;

    assert(m_programDescriptor);

    glUseProgram(m_programDescriptor);
}

void GpuProgram::unbind()
{
    glUseProgram(0);
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

GpuProgramManualLoader::GpuProgramManualLoader(const std::string &guid, const std::string &vertexData, const std::string &fragmentData)
    : m_resourceGuid(guid),
    m_vertexData(vertexData),
    m_fragmentData(fragmentData)
{

}

GpuProgramManualLoader::GpuProgramManualLoader(const std::string &vertexShaderPath, const std::string &fragmentShaderPath)
    : m_resourceGuid(vertexShaderPath + ";" + fragmentShaderPath),
    m_vertexShaderPath(vertexShaderPath),
    m_fragmentShaderPath(fragmentShaderPath)
{
    assert(vertexShaderPath != fragmentShaderPath);
}

GpuProgram* GpuProgramManualLoader::load()
{
    shared_ptr<Shader> vertexShader, fragmentShader;

    if (!m_vertexShaderPath.empty() && !m_fragmentShaderPath.empty())
    {
        vertexShader = Shader::createFromFile(m_vertexShaderPath);
        fragmentShader = Shader::createFromFile(m_fragmentShaderPath);
    }
    else
    {
        vertexShader = Shader::createFromString(m_vertexData, Shader::Type::VERTEX);
        fragmentShader = Shader::createFromString(m_fragmentData, Shader::Type::FRAGMENT);
    }

    auto program = new GpuProgram({ vertexShader, fragmentShader });

    program->setGUID(m_resourceGuid);

    return program;
}

} }
