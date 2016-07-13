#include "GpuProgram.h"

#include <df3d/engine/EngineController.h>
#include <df3d/engine/io/DataSource.h>
#include <df3d/engine/io/DefaultFileSystem.h>
#include <df3d/engine/io/FileSystemHelpers.h>
#include <df3d/engine/render/RenderManager.h>
#include <df3d/engine/render/IRenderBackend.h>
#include <df3d/engine/render/gl/GLSLPreprocess.h>

namespace df3d {

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

    return svc().renderManager().getBackend().createShader(shaderType, GLSLPreprocess::preprocess(buffer, file->getPath()));
}

ShaderHandle GpuProgramManualLoader::createShaderFromString(const std::string &data, ShaderType shaderType) const
{
    if (shaderType == ShaderType::UNDEFINED)
    {
        DFLOG_WARN("Failed to create shader of UNDEFINED type");
        return{};
    }

    return svc().renderManager().getBackend().createShader(shaderType, GLSLPreprocess::preprocess(data));
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
