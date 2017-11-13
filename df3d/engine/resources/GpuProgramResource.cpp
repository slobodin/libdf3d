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
#include <df3d/lib/Utils.h>

namespace df3d {

namespace {

SharedUniformType GetSharedTypeForUniform(const std::string &name)
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

#ifdef DF3D_IOS
#error "implement"
#endif

    return SharedUniformType::COUNT;
}

void InitProgramUniforms(GpuProgramResource *gpuProgram, std::vector<std::string> uniformNames)
{
    auto &backend = svc().renderManager().getBackend();
    bool glBackend = backend.getID() == RenderBackendID::GL;

    for (const auto &uniName : uniformNames)
    {
        auto uniformHandle = backend.getUniform(gpuProgram->handle, uniName.c_str());
        if (!uniformHandle.isValid())
        {
            DF3D_ASSERT(false);
            continue;
        }

        auto sharedType = GetSharedTypeForUniform(uniName);
        if (sharedType != SharedUniformType::COUNT)
        {
            SharedUniform suni;
            suni.handle = uniformHandle;
            suni.type = sharedType;

            gpuProgram->sharedUniforms.push_back(suni);
        }
        else
        {
            Id customUniformStrId(uniName.c_str());
            gpuProgram->customUniforms[customUniformStrId] = uniformHandle;
        }
    }
}

void InitProgramUniformsMetal(GpuProgramResource *gpuProgram, std::vector<std::string> allUniformNames)
{
#ifdef DF3D_IOS
#error "IMPLEMENT"
    std::vector<std::string> customUniformNames;

    for (const auto &name : allUniformNames)
    {
        auto sharedType = GetSharedTypeForUniform(name, true);
        if (sharedType != SharedUniformType::COUNT)
        {
            SharedUniform suni;
            suni.handle = {};
            suni.type = sharedType;

            gpuProgram->sharedUniforms.push_back(suni);
        }
        else
        {
            customUniformNames.push_back(name);
        }
    }

    std::vector<UniformHandle> uniforms;
    svc().renderManager().getBackend().requestUniforms(gpuProgram->handle, uniforms, customUniformNames);

    DF3D_ASSERT(uniforms.size() == customUniformNames.size());

    for (size_t i = 0; i < uniforms.size(); i++)
    {
        Id customUniformStrId(customUniformNames[i].c_str());
        gpuProgram->customUniforms[customUniformStrId] = uniforms[i];
    }
#endif
}

}

bool GpuProgramHolder::decodeStartup(ResourceDataSource &dataSource, Allocator &allocator)
{
    auto root = JsonUtils::fromFile(dataSource);
    if (root.isNull())
        return false;

    DF3D_ASSERT(root.isMember("uniformNames"));

    const auto &jsonUniformNames = root["uniformNames"];
    for (const auto &jsonUniformName : jsonUniformNames)
    {
        m_uniformNames.push_back(jsonUniformName.asString());
    }

    auto backendID = svc().renderManager().getBackendID();
    if (backendID == RenderBackendID::GL)
    {
        DF3D_ASSERT(root.isMember("vertex") && root.isMember("fragment"));

        m_vShaderPath = root["vertex"].asString();
        m_fShaderPath = root["fragment"].asString();
    }
    else
    {
        DF3D_ASSERT(root.isMember("metal_backend_data"));

        const auto &jsonMetalData = root["metal_backend_data"];

        m_vShaderPath = jsonMetalData["vertexFunctionName"].asString();
        m_fShaderPath = jsonMetalData["fragmentFunctionName"].asString();
    }

    return !m_vShaderPath.empty() && !m_fShaderPath.empty();
}

void GpuProgramHolder::decodeCleanup(Allocator &allocator)
{

}

bool GpuProgramHolder::createResource(Allocator &allocator)
{
    auto backendID = svc().renderManager().getBackendID();
    if (backendID == RenderBackendID::GL)
    {
        auto getShaderData = [](const std::string &filePath) {
            std::string result;

            if (auto file = svc().resourceManager().getFS().open(filePath.c_str()))
            {
                result.resize(file->getSize(), 0);
                file->read(&result[0], result.size());

                svc().resourceManager().getFS().close(file);
            }

            return result;
        };

        m_resource = CreateGPUProgramFromData(getShaderData(m_vShaderPath), getShaderData(m_fShaderPath),
                                              m_uniformNames, allocator,
                                              m_vShaderPath, m_fShaderPath);
    }
    else if (backendID == RenderBackendID::METAL)
    {
        m_resource = CreateGPUProgramMetal(m_vShaderPath, m_fShaderPath, m_uniformNames, allocator);
    }

    return m_resource != nullptr;
}

void GpuProgramHolder::destroyResource(Allocator &allocator)
{
    svc().renderManager().getBackend().destroyGPUProgram(m_resource->handle);
    MAKE_DELETE(allocator, m_resource);
    m_resource = nullptr;
}

GpuProgramResource* CreateGPUProgramFromData(const std::string &vShaderData, const std::string &fShaderData,
                                             std::vector<std::string> uniformNames, Allocator &alloc,
                                             std::string vShaderPath, std::string fShaderPath)
{
    auto vShaderDataProcessed = GLSLPreprocess::preprocess(vShaderData, vShaderPath);
    auto fShaderDataProcessed = GLSLPreprocess::preprocess(fShaderData, fShaderPath);

    auto gpuProgramHandle = svc().renderManager().getBackend().createGPUProgram(vShaderDataProcessed.c_str(), fShaderDataProcessed.c_str());
    if (!gpuProgramHandle.isValid())
        return nullptr;

    auto resource = MAKE_NEW(alloc, GpuProgramResource)(alloc);
    resource->handle = gpuProgramHandle;

    InitProgramUniforms(resource, uniformNames);

    return resource;
}

GpuProgramResource* CreateGPUProgramMetal(const std::string &vShaderFunction, const std::string &fShaderFunction, std::vector<std::string> uniformNames, Allocator &alloc)
{
#ifdef DF3D_IOS
#error "implement"
    auto &backend = svc().renderManager().getBackend();
    auto gpuProgramHandle = backend.createGPProgramMetal(vShaderFunction.c_str(), fShaderFunction.c_str());
    if (!gpuProgramHandle.isValid())
        return nullptr;

    auto resource = MAKE_NEW(alloc, GpuProgramResource)(alloc);
    resource->handle = gpuProgramHandle;

    InitProgramUniformsMetal(resource, uniformNames);

    return resource;
#endif
    return nullptr;
}

}
