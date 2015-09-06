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

std::string Shader::preprocess(const std::string &shaderData)
{
#ifdef DF3D_DESKTOP
    std::string versionPrefix = "#version 110\n";
#else
    std::string versionPrefix = "";
#endif

    std::string precisionPrefix = "#ifdef GL_ES\n"
        "#define LOWP lowp\n"
        "precision mediump float;\n"
        "#else\n"
        "#define LOWP\n"
        "#endif\n";

    std::string result = versionPrefix + precisionPrefix + shaderData;

    return result;
}

std::string Shader::preprocessInclude(std::string shaderData, const std::string &shaderFilePath)
{
    const std::string shaderDirectory = g_fileSystem->getFileDirectory(shaderFilePath);
    const std::string INCLUDE_DIRECTIVE = "#include";
    const size_t INCLUDE_DIRECTIVE_LEN = INCLUDE_DIRECTIVE.size();

    size_t found = shaderData.find(INCLUDE_DIRECTIVE, 0);
    while (found != std::string::npos)
    {
        auto start = shaderData.find('\"', found + INCLUDE_DIRECTIVE_LEN);
        auto end = shaderData.find('\"', start + 1);

        if (start == end || start == std::string::npos || end == std::string::npos)
        {
            base::glog << "Failed to preprocess shader: invalid include directive" << base::logwarn;
            return shaderData;
        }

        auto fileToInclude = shaderData.substr(start + 1, end - start - 1);
        if (fileToInclude.empty())
        {
            base::glog << "Failed to preprocess shader: empty include path" << base::logwarn;
            return shaderData;
        }

        fileToInclude = resources::FileSystem::pathConcatenate(shaderDirectory, fileToInclude);
        auto file = g_fileSystem->openFile(fileToInclude);
        if (!file || !file->valid())
        {
            base::glog << "Failed to preprocess shader: file" << fileToInclude << "not found" << base::logwarn;
            return shaderData;
        }

        std::string includeData(file->getSize(), 0);
        file->getRaw(&includeData[0], file->getSize());

        shaderData.replace(found, end - found + 1, includeData);

        found = shaderData.find(INCLUDE_DIRECTIVE, found);
    }

    return shaderData;
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

    const char *pdata = m_shaderData.c_str();
    glShaderSource(m_shaderDescriptor, 1, &pdata, nullptr);
    glCompileShader(m_shaderDescriptor);

    int compileOk;
    glGetShaderiv(m_shaderDescriptor, GL_COMPILE_STATUS, &compileOk);
    if (compileOk == GL_FALSE)
    {
        base::glog << "Failed to compile a shader" << base::logwarn;
        base::glog << "\n" << m_shaderData << base::logmess;
        shaderLog(m_shaderDescriptor);

        return false;
    }

    setCompiled(true);

    return true;
}

void Shader::setShaderData(const std::string &data)
{
    m_shaderData = data;
    setCompiled(false);
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

shared_ptr<Shader> Shader::createFromFile(const std::string &filePath)
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
        shader->setType(Type::VERTEX);
    else if (ext == ".frag")
        shader->setType(Type::FRAGMENT);
    else
    {
        base::glog << "Can not decode GLSL shader because it's type is undefined" << filePath << base::logwarn;
        return nullptr;
    }

    std::string buffer(file->getSize(), 0);
    file->getRaw(&buffer[0], file->getSize());

    shader->setShaderData(preprocessInclude(preprocess(buffer), file->getPath()));

    return shader;
}

shared_ptr<Shader> Shader::createFromString(const std::string &shaderData, Type type)
{
    if (type == Type::UNDEFINED)
    {
        base::glog << "Failed to create shader of UNDEFINED type" << base::logwarn;
        return nullptr;
    }

    auto shader = make_shared<Shader>(type);
    shader->setShaderData(preprocess(shaderData));
    return shader;
}

} }
