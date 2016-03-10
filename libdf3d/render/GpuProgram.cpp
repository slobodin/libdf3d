#include "GpuProgram.h"

#include <libdf3d/base/EngineController.h>
#include <libdf3d/io/FileDataSource.h>
#include <libdf3d/io/FileSystem.h>
#include <libdf3d/render/RenderManager.h>
#include <libdf3d/render/IRenderBackend.h>

namespace df3d {

static std::string ShaderPreprocess(const std::string &shaderData)
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

    return versionPrefix + precisionPrefix + shaderData;
}

static std::string ShaderPreprocessInclude(std::string shaderData, const std::string &shaderFilePath)
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

static SharedUniformType GetSharedTypeForUniform(const std::string &name)
{
    if (name == "u_worldViewProjectionMatrix")
        return SharedUniformType::WORLD_VIEW_PROJECTION_MATRIX_UNIFORM;
    else if (name == "u_worldViewMatrix")
        return SharedUniformType::WORLD_VIEW_MATRIX_UNIFORM;
    else if (name == "u_worldViewMatrix3x3")
        return SharedUniformType::WORLD_VIEW_3X3_MATRIX_UNIFORM;
    else if (name == "u_viewMatrixInverse")
        return SharedUniformType::VIEW_INVERSE_MATRIX_UNIFORM;
    else if (name == "u_viewMatrix")
        return SharedUniformType::VIEW_MATRIX_UNIFORM;
    else if (name == "u_projectionMatrix")
        return SharedUniformType::PROJECTION_MATRIX_UNIFORM;
    else if (name == "u_worldMatrix")
        return SharedUniformType::WORLD_MATRIX_UNIFORM;
    else if (name == "u_worldMatrixInverse")
        return SharedUniformType::WORLD_INVERSE_MATRIX_UNIFORM;
    else if (name == "u_normalMatrix")
        return SharedUniformType::NORMAL_MATRIX_UNIFORM;
    else if (name == "u_globalAmbient")
        return SharedUniformType::GLOBAL_AMBIENT_UNIFORM;
    else if (name == "u_cameraPosition")
        return SharedUniformType::CAMERA_POSITION_UNIFORM;
    else if (name == "u_fogDensity")
        return SharedUniformType::FOG_DENSITY_UNIFORM;
    else if (name == "u_fogColor")
        return SharedUniformType::FOG_COLOR_UNIFORM;
    else if (name == "u_pixelSize")
        return SharedUniformType::PIXEL_SIZE_UNIFORM;
    else if (name == "u_elapsedTime")
        return SharedUniformType::ELAPSED_TIME_UNIFORM;
    else if (name == "current_light.diffuse")
        return SharedUniformType::SCENE_LIGHT_DIFFUSE_UNIFORM;
    else if (name == "current_light.specular")
        return SharedUniformType::SCENE_LIGHT_SPECULAR_UNIFORM;
    else if (name == "current_light.position")
        return SharedUniformType::SCENE_LIGHT_POSITION_UNIFORM;
    else if (name == "current_light.constantAttenuation")
        return SharedUniformType::SCENE_LIGHT_KC_UNIFORM;
    else if (name == "current_light.linearAttenuation")
        return SharedUniformType::SCENE_LIGHT_KL_UNIFORM;
    else if (name == "current_light.quadraticAttenuation")
        return SharedUniformType::SCENE_LIGHT_KQ_UNIFORM;

    return SharedUniformType::COUNT;
}

GpuProgram::GpuProgram(GpuProgramDescriptor descr)
    : m_descriptor(descr)
{
    assert(m_descriptor.valid());

    std::vector<UniformDescriptor> uniforms;
    std::vector<std::string> uniformNames;

    svc().renderManager().getBackend().requestUniforms(m_descriptor, uniforms, uniformNames);

    // Sanity check.
    assert(uniforms.size() == uniformNames.size());

    for (size_t i = 0; i < uniforms.size(); i++)
    {
        auto sharedType = GetSharedTypeForUniform(uniformNames[i]);
        if (sharedType != SharedUniformType::COUNT)
        {
            SharedUniform suni;
            suni.descr = uniforms[i];
            suni.type = sharedType;

            m_sharedUniforms.push_back(suni);
        }
        else
        {
            m_customUniforms[uniformNames[i]] = uniforms[i];
        }
    }
}

GpuProgram::~GpuProgram()
{
    svc().renderManager().getBackend().destroyGpuProgram(m_descriptor);
}

UniformDescriptor GpuProgram::getCustomUniform(const std::string &name)
{
    auto found = m_customUniforms.find(name);
    if (found != m_customUniforms.end())
        return found->second;
    return{};
}

std::vector<std::string> GpuProgram::getCustomUniformNames() const
{
    std::vector<std::string> result;
    for (const auto &kv : m_customUniforms)
        result.push_back(kv.first);
    return result;
}

ShaderDescriptor GpuProgramManualLoader::createShaderFromFile(const std::string &path) const
{
    auto file = svc().fileSystem().openFile(path);
    if (!file)
    {
        glog << "Can not create shader. File" << path << "doesn't exist" << logwarn;
        return{};
    }

    const std::string &ext = svc().fileSystem().getFileExtension(path);

    ShaderType shaderType;

    if (ext == ".vert")
        shaderType = ShaderType::VERTEX;
    else if (ext == ".frag")
        shaderType = ShaderType::FRAGMENT;
    else
    {
        glog << "Can not decode a shader because it's type is undefined" << path << logwarn;
        return{};
    }

    std::string buffer(file->getSizeInBytes(), 0);
    file->getRaw(&buffer[0], buffer.size());

    auto shaderStr = ShaderPreprocessInclude(ShaderPreprocess(buffer), file->getPath());

    return svc().renderManager().getBackend().createShader(shaderType, shaderStr);
}

ShaderDescriptor GpuProgramManualLoader::createShaderFromString(const std::string &data, ShaderType shaderType) const
{
    if (shaderType == ShaderType::UNDEFINED)
    {
        glog << "Failed to create shader of UNDEFINED type" << logwarn;
        return{};
    }

    return svc().renderManager().getBackend().createShader(shaderType, ShaderPreprocess(data));
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
    ShaderDescriptor vertexShader, fragmentShader;

    if (!m_vertexShaderPath.empty() && !m_fragmentShaderPath.empty())
    {
        vertexShader = createShaderFromFile(m_vertexShaderPath);
        fragmentShader = createShaderFromFile(m_fragmentShaderPath);
    }
    else
    {
        vertexShader = createShaderFromString(m_vertexData, ShaderType::VERTEX);
        fragmentShader = createShaderFromString(m_fragmentData, ShaderType::FRAGMENT);
    }

    auto gpuProgramDescriptor = svc().renderManager().getBackend().createGpuProgram(vertexShader, fragmentShader);

    auto program = new GpuProgram(gpuProgramDescriptor);
    program->setGUID(m_resourceGuid);

    svc().renderManager().getBackend().destroyShader(vertexShader);
    svc().renderManager().getBackend().destroyShader(fragmentShader);
    return program;
}

}
