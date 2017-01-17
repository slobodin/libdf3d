#include "GpuProgramResource.h"

#include <df3d/engine/EngineController.h>
#include <df3d/engine/render/RenderManager.h>
#include <df3d/engine/render/IRenderBackend.h>
#include <df3d/engine/render/gl/GLSLPreprocess.h>
#include <df3d/engine/resources/ResourceManager.h>
#include <df3d/engine/resources/ResourceFileSystem.h>
#include <df3d/engine/resources/ResourceDataSource.h>
#include <df3d/engine/io/FileSystemHelpers.h>
#include <df3d/lib/JsonUtils.h>

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
    else if (name == "light_0.color")
        return SharedUniformType::SCENE_LIGHT_0_COLOR_UNIFORM;
    else if (name == "light_0.position")
        return SharedUniformType::SCENE_LIGHT_0_POSITION_UNIFORM;
    else if (name == "light_1.color")
        return SharedUniformType::SCENE_LIGHT_1_COLOR_UNIFORM;
    else if (name == "light_1.position")
        return SharedUniformType::SCENE_LIGHT_1_POSITION_UNIFORM;

    return SharedUniformType::COUNT;
}

static ShaderHandle CreateShaderFromFile(const char *path)
{
    ShaderHandle result;

    if (auto file = svc().resourceManager().getFS().open(path))
    {
        ShaderType shaderType = ShaderType::UNDEFINED;
        if (FileSystemHelpers::compareExtension(path, ".vert"))
            shaderType = ShaderType::VERTEX;
        else if (FileSystemHelpers::compareExtension(path, ".frag"))
            shaderType = ShaderType::FRAGMENT;
        else
            DFLOG_WARN("Can not decode a shader because it's type is undefined: %s", path);

        if (shaderType != ShaderType::UNDEFINED)
        {
            std::string buffer(file->getSize(), 0);
            file->read(&buffer[0], buffer.size());

            result = svc().renderManager().getBackend().createShader(shaderType, GLSLPreprocess::preprocess(buffer, path).c_str());
        }

        svc().resourceManager().getFS().close(file);
    }

    return result;
}

static ShaderHandle CreateShaderFromString(const std::string &data, ShaderType shaderType)
{
    DF3D_ASSERT(shaderType != ShaderType::UNDEFINED);

    return svc().renderManager().getBackend().createShader(shaderType, GLSLPreprocess::preprocess(data).c_str());
}

GpuProgramResource* CreateGpuProgram(ShaderHandle vShader, ShaderHandle fShader, Allocator &allocator)
{
    if (!(vShader.isValid() && fShader.isValid()))
        return nullptr;       // FIXME: destroy valid shader.

    auto gpuProgramHandle = svc().renderManager().getBackend().createGpuProgram(vShader, fShader);
    if (!gpuProgramHandle.isValid())
        return nullptr;

    auto resource = MAKE_NEW(allocator, GpuProgramResource)(allocator);
    resource->handle = gpuProgramHandle;

    std::vector<UniformHandle> uniforms;
    std::vector<std::string> uniformNames;

    svc().renderManager().getBackend().requestUniforms(resource->handle, uniforms, uniformNames);

    DF3D_ASSERT(uniforms.size() == uniformNames.size());

    for (size_t i = 0; i < uniforms.size(); i++)
    {
        auto sharedType = GetSharedTypeForUniform(uniformNames[i]);
        if (sharedType != SharedUniformType::COUNT)
        {
            SharedUniform suni;
            suni.handle = uniforms[i];
            suni.type = sharedType;

            resource->sharedUniforms.push_back(suni);
        }
        else
        {
            Id customUniformStrId(uniformNames[i].c_str());
            resource->customUniforms[customUniformStrId] = uniforms[i];
        }
    }

    return resource;
}

bool GpuProgramHolder::decodeStartup(ResourceDataSource &dataSource, Allocator &allocator)
{
    auto root = JsonUtils::fromFile(dataSource);
    if (root.isNull())
        return false;

    DF3D_ASSERT(root.isMember("vertex") && root.isMember("fragment"));

    m_vShaderPath = root["vertex"].asString();
    m_fShaderPath = root["fragment"].asString();

    return !m_vShaderPath.empty() && !m_fShaderPath.empty();
}

void GpuProgramHolder::decodeCleanup(Allocator &allocator)
{

}

bool GpuProgramHolder::createResource(Allocator &allocator)
{
    ShaderHandle vertexShader = CreateShaderFromFile(m_vShaderPath.c_str());
    ShaderHandle fragmentShader = CreateShaderFromFile(m_fShaderPath.c_str());

    m_resource = CreateGpuProgram(vertexShader, fragmentShader, allocator);

    return m_resource != nullptr;
}

void GpuProgramHolder::destroyResource(Allocator &allocator)
{
    svc().renderManager().getBackend().destroyGpuProgram(m_resource->handle);
    MAKE_DELETE(allocator, m_resource);
    m_resource = nullptr;
}

GpuProgramResource* GpuProgramFromData(const std::string &vShaderData, const std::string &fShaderData, Allocator &alloc)
{
    ShaderHandle vertexShader = CreateShaderFromString(vShaderData, ShaderType::VERTEX);
    ShaderHandle fragmentShader = CreateShaderFromString(fShaderData, ShaderType::FRAGMENT);

    return CreateGpuProgram(vertexShader, fragmentShader, alloc);
}

}
