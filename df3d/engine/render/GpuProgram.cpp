#include "GpuProgram.h"

#include <cctype>
#include <df3d/engine/EngineController.h>
#include <df3d/engine/io/DataSource.h>
#include <df3d/engine/io/DefaultFileSystem.h>
#include <df3d/engine/io/FileSystemHelpers.h>
#include <df3d/engine/render/MaterialLib.h>
#include <df3d/engine/render/RenderManager.h>
#include <df3d/engine/render/IRenderBackend.h>
#include <df3d/lib/Utils.h>

namespace df3d {

static std::string ShaderIfdefsParse(const std::string &shaderData)
{
    const std::string IFDEF_DIRECTIVE = "#ifdef";
    const std::string ENDIF_DIRECTIVE = "#endif";
    std::string res = shaderData;

    size_t lastFoundIfdef = res.find(IFDEF_DIRECTIVE);
    while (lastFoundIfdef != std::string::npos)
    {
        auto found = std::find_if(res.begin() + lastFoundIfdef + IFDEF_DIRECTIVE.size(), res.end(), [](char ch) { return !std::isspace(ch); });
        if (found == res.end())
            return shaderData;

        auto foundEndif = res.find_first_of('#', std::distance(res.begin(), found));
        if (foundEndif == std::string::npos)
            return shaderData;

        auto defineEnd = std::find_if(found, res.begin() + foundEndif, [](char ch) { return std::isspace(ch); });

        std::string defineToken = std::string(found, defineEnd);

        if (!df3d::utils::contains_key(MaterialLib::SHADER_DEFINES, defineToken))
        {
            auto beginErase = res.begin() + lastFoundIfdef;
            auto endErase = res.begin() + foundEndif + ENDIF_DIRECTIVE.size();

            res.erase(beginErase, endErase);
        }
        else
        {
            auto beginErase = res.begin() + lastFoundIfdef;

            res.erase(beginErase, defineEnd);

            foundEndif = res.find_first_of('#');        // Should be #endif

            res.erase(foundEndif, ENDIF_DIRECTIVE.size());
        }

        lastFoundIfdef = res.find(IFDEF_DIRECTIVE, 0);
    }

    return res;
}

static std::string ShaderPreprocess(const std::string &shaderData)
{
#ifdef DF3D_DESKTOP
    std::string versionPrefix = "#version 110\n";
#else
    std::string versionPrefix = "";
#endif

    std::string precisionPrefix = "#ifdef GL_ES\n"
        "#define LOWP lowp\n"
        "#define MEDIUMP mediump\n"
        "precision highp float;\n"
        "#else\n"
        "#define LOWP\n"
        "#define MEDIUMP\n"
        "#endif\n";

    return versionPrefix + precisionPrefix + ShaderIfdefsParse(shaderData);
}

static std::string ShaderPreprocessInclude(std::string shaderData, const std::string &shaderFilePath)
{
    const std::string shaderDirectory = FileSystemHelpers::getFileDirectory(shaderFilePath);
    const std::string INCLUDE_DIRECTIVE = "#include";
    const size_t INCLUDE_DIRECTIVE_LEN = INCLUDE_DIRECTIVE.size();

    size_t found = shaderData.find(INCLUDE_DIRECTIVE, 0);
    while (found != std::string::npos)
    {
        auto start = shaderData.find('\"', found + INCLUDE_DIRECTIVE_LEN);
        auto end = shaderData.find('\"', start + 1);

        if (start == end || start == std::string::npos || end == std::string::npos)
        {
            DFLOG_WARN("Failed to preprocess a shader: invalid include directive");
            return shaderData;
        }

        auto fileToInclude = shaderData.substr(start + 1, end - start - 1);
        if (fileToInclude.empty())
        {
            DFLOG_WARN("Failed to preprocess a shader: empty include path");
            return shaderData;
        }

        fileToInclude = FileSystemHelpers::pathConcatenate(shaderDirectory, fileToInclude);
        auto file = svc().fileSystem().open(fileToInclude);
        if (!file)
        {
            DFLOG_WARN("Failed to preprocess a shader: file %s is not found", fileToInclude.c_str());
            return shaderData;
        }

        std::string includeData(file->getSize(), 0);
        file->read(&includeData[0], includeData.size());

        shaderData.replace(found, end - found + 1, includeData);

        found = shaderData.find(INCLUDE_DIRECTIVE, 0);
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

GpuProgram::GpuProgram(GpuProgramHandle handle)
    : m_handle(handle)
{
    DF3D_ASSERT_MESS(m_handle.valid(), "invalid GPU program hanlde");

    std::vector<UniformHandle> uniforms;
    std::vector<std::string> uniformNames;

    svc().renderManager().getBackend().requestUniforms(m_handle, uniforms, uniformNames);

    DF3D_ASSERT(uniforms.size() == uniformNames.size());

    for (size_t i = 0; i < uniforms.size(); i++)
    {
        auto sharedType = GetSharedTypeForUniform(uniformNames[i]);
        if (sharedType != SharedUniformType::COUNT)
        {
            SharedUniform suni;
            suni.handle = uniforms[i];
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
    svc().renderManager().getBackend().destroyGpuProgram(m_handle);
}

UniformHandle GpuProgram::getCustomUniform(const std::string &name)
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

ShaderHandle GpuProgramManualLoader::createShaderFromFile(const std::string &path) const
{
    auto file = svc().fileSystem().open(path);
    if (!file)
    {
        DFLOG_WARN("Can not create a shader. File %s doesn't exist", path.c_str());
        return{};
    }

    const auto ext = FileSystemHelpers::getFileExtension(path);

    ShaderType shaderType;

    if (ext == ".vert")
        shaderType = ShaderType::VERTEX;
    else if (ext == ".frag")
        shaderType = ShaderType::FRAGMENT;
    else
    {
        DFLOG_WARN("Can not decode a shader because it's type is undefined: %s", path.c_str());
        return{};
    }

    std::string buffer(file->getSize(), 0);
    file->read(&buffer[0], buffer.size());

    auto shaderStr = ShaderPreprocess(ShaderPreprocessInclude(buffer, file->getPath()));

    return svc().renderManager().getBackend().createShader(shaderType, shaderStr);
}

ShaderHandle GpuProgramManualLoader::createShaderFromString(const std::string &data, ShaderType shaderType) const
{
    if (shaderType == ShaderType::UNDEFINED)
    {
        DFLOG_WARN("Failed to create shader of UNDEFINED type");
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
    DF3D_ASSERT(vertexShaderPath != fragmentShaderPath);
}

GpuProgram* GpuProgramManualLoader::load()
{
    ShaderHandle vertexShader, fragmentShader;

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

    if (!(vertexShader.valid() && fragmentShader.valid()))
        return nullptr;

    auto gpuProgramHandle = svc().renderManager().getBackend().createGpuProgram(vertexShader, fragmentShader);
    if (!gpuProgramHandle.valid())
        return nullptr;

    auto program = new GpuProgram(gpuProgramHandle);
    program->setGUID(m_resourceGuid);

    return program;
}

}
