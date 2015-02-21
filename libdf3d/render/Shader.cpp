#include "df3d_pch.h"
#include "Shader.h"

#include "OpenGLCommon.h"
#include <base/SystemsMacro.h>
#include <resources/FileDataSource.h>

namespace df3d { namespace render {

void shaderLog(unsigned int shader)
{
    int infologLen = 0;
    char *infoLog = nullptr;

    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infologLen);
    infoLog = new char[infologLen + 1];

    glGetShaderInfoLog(shader, infologLen, nullptr, infoLog);
    base::glog << "Shader info log:" << infoLog << base::logmess;

    delete [] infoLog;
}

void Shader::createGLShader()
{
    if (m_type == Type::VERTEX)
        m_shaderDescriptor = glCreateShader(GL_VERTEX_SHADER);
    else if (m_type == Type::FRAGMENT)
        m_shaderDescriptor = glCreateShader(GL_FRAGMENT_SHADER);
    else
        m_shaderDescriptor = 0;
}

Shader::Shader(Type type)
    : m_type(type)
{
}

Shader::~Shader()
{
    if (m_shaderDescriptor != 0)
        glDeleteShader(m_shaderDescriptor);
}

bool Shader::compile()
{
    if (m_isCompiled)
        return true;

    // Try to create shader.
    if (m_shaderDescriptor == 0)
        createGLShader();

    if (m_shaderDescriptor == 0 || m_type == Type::UNDEFINED)
    {
        base::glog << "Can not compile GLSL shader due to undefined type" << base::logwarn;
        return false;
    }

    if (m_shaderData.empty())
    {
        base::glog << "Empty shader data" << base::logwarn;
        return false;
    }

    auto precisionPrefix = "#ifdef GL_ES\n"
        "#define LOWP lowp\n"
        "precision mediump float;\n"
        "#else\n"
        "#define LOWP\n"
        "#endif\n";

    std::string shaderData = precisionPrefix;
    shaderData += m_shaderData;

#ifdef DF3D_WINDOWS
    const char *src[2] = { "#version 110\n", shaderData.c_str() };
    glShaderSource(m_shaderDescriptor, 2, src, nullptr);
#else
    const char *pdata = shaderData.c_str();
    glShaderSource(m_shaderDescriptor, 1, &pdata, nullptr);
#endif
    glCompileShader(m_shaderDescriptor);

    int compileOk;
    glGetShaderiv(m_shaderDescriptor, GL_COMPILE_STATUS, &compileOk);
    if (compileOk == GL_FALSE)
    {
        base::glog << "Failed to compile shader" << base::logwarn;
        shaderLog(m_shaderDescriptor);
        return false;
    }

    setCompiled(true);

    return true;
}

bool Shader::compiled()
{
    return m_isCompiled;
}

void Shader::setCompiled(bool isCompiled)
{
    m_isCompiled = isCompiled;
}

void Shader::setShaderDescriptor(unsigned int descr)
{
    m_shaderDescriptor = descr;
}

void Shader::setShaderData(const std::string &data)
{
    m_shaderData = data;
    setCompiled(false);
}

void Shader::setShaderData(const char **data, size_t lnCount)
{
    m_shaderData = "";

    for (size_t i = 0; i < lnCount; i++)
    {
        m_shaderData.append(data[i]);
        m_shaderData.append("\n");
    }
}

void Shader::setType(Type type)
{
    m_type = type;
}

Shader::Type Shader::getType() const
{
    return m_type;
}

unsigned int Shader::getDescriptor() const
{
    return m_shaderDescriptor;
}

shared_ptr<Shader> Shader::createFromFile(const char *filePath)
{
    auto shader = make_shared<Shader>();
    auto file = g_fileSystem->openFile(filePath);
    if (!file || !file->valid())
    {
        base::glog << "Can not create shader. File" << filePath << "doesn't exist" << base::logwarn;
        return nullptr;
    }

    const std::string &ext = g_fileSystem->getFileExtension(filePath);

    if (ext == ".vert")
        shader->setType(render::Shader::Type::VERTEX);
    else if (ext == ".frag")
        shader->setType(render::Shader::Type::FRAGMENT);
    else
    {
        base::glog << "Can not decode GLSL shader because it's type is undefined" << filePath << base::logwarn;
        return nullptr;
    }

    std::string buffer(file->getSize(), 0);
    file->getRaw(&buffer[0], file->getSize());

    shader->setShaderData(buffer);

    return shader;
}

} }
