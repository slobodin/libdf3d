#include "Shader.h"

#include "OpenGLCommon.h"
#include <libdf3d/base/EngineController.h>
#include <libdf3d/io/FileSystem.h>
#include <libdf3d/io/FileDataSource.h>

namespace df3d {

void shaderLog(unsigned int shader)
{
    int infologLen = 0;
    char *infoLog = nullptr;

    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infologLen);
    infoLog = new char[infologLen + 1];

    glGetShaderInfoLog(shader, infologLen, nullptr, infoLog);
    glog << "Shader info log:" << infoLog << logmess;

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
    const std::string shaderDirectory = svc().fileSystem().getFileDirectory(shaderFilePath);
    const std::string INCLUDE_DIRECTIVE = "#include";
    const size_t INCLUDE_DIRECTIVE_LEN = INCLUDE_DIRECTIVE.size();

    size_t found = shaderData.find(INCLUDE_DIRECTIVE, 0);
    while (found != std::string::npos)
    {
        auto start = shaderData.find('\"', found + INCLUDE_DIRECTIVE_LEN);
        auto end = shaderData.find('\"', start + 1);

        if (start == end || start == std::string::npos || end == std::string::npos)
        {
            glog << "Failed to preprocess shader: invalid include directive" << logwarn;
            return shaderData;
        }

        auto fileToInclude = shaderData.substr(start + 1, end - start - 1);
        if (fileToInclude.empty())
        {
            glog << "Failed to preprocess shader: empty include path" << logwarn;
            return shaderData;
        }

        fileToInclude = FileSystem::pathConcatenate(shaderDirectory, fileToInclude);
        auto file = svc().fileSystem().openFile(fileToInclude);
        if (!file || !file->valid())
        {
            glog << "Failed to preprocess shader: file" << fileToInclude << "not found" << logwarn;
            return shaderData;
        }

        std::string includeData(file->getSizeInBytes(), 0);
        file->getRaw(&includeData[0], includeData.size());

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
        glog << "Can not compile GLSL shader due to undefined type" << logwarn;
        return false;
    }

    if (m_shaderData.empty())
    {
        glog << "Empty shader data" << logwarn;
        return false;
    }

    const char *pdata = m_shaderData.c_str();
    glShaderSource(m_shaderDescriptor, 1, &pdata, nullptr);
    glCompileShader(m_shaderDescriptor);

    int compileOk;
    glGetShaderiv(m_shaderDescriptor, GL_COMPILE_STATUS, &compileOk);
    if (compileOk == GL_FALSE)
    {
        glog << "Failed to compile a shader" << logwarn;
        glog << "\n" << m_shaderData << logmess;
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
    auto file = svc().fileSystem().openFile(filePath);
    if (!file || !file->valid())
    {
        glog << "Can not create shader. File" << filePath << "doesn't exist" << logwarn;
        return nullptr;
    }

    const std::string &ext = svc().fileSystem().getFileExtension(filePath);

    if (ext == ".vert")
        shader->setType(Type::VERTEX);
    else if (ext == ".frag")
        shader->setType(Type::FRAGMENT);
    else
    {
        glog << "Can not decode GLSL shader because it's type is undefined" << filePath << logwarn;
        return nullptr;
    }

    std::string buffer(file->getSizeInBytes(), 0);
    file->getRaw(&buffer[0], buffer.size());

    shader->setShaderData(preprocessInclude(preprocess(buffer), file->getPath()));

    return shader;
}

shared_ptr<Shader> Shader::createFromString(const std::string &shaderData, Type type)
{
    if (type == Type::UNDEFINED)
    {
        glog << "Failed to create shader of UNDEFINED type" << logwarn;
        return nullptr;
    }

    auto shader = make_shared<Shader>(type);
    shader->setShaderData(preprocess(shaderData));
    return shader;
}

}
