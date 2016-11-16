#include "MaterialResource.h"

#include <df3d/engine/EngineController.h>
#include <df3d/engine/resources/ResourceManager.h>
#include <df3d/engine/resources/ResourceDataSource.h>
#include <df3d/engine/resources/ResourceFileSystem.h>
#include <df3d/engine/resources/TextureResource.h>
#include <df3d/engine/resources/GpuProgramResource.h>
#include <df3d/engine/render/Material.h>
#include <df3d/engine/render/RenderManager.h>
#include <df3d/lib/Utils.h>
#include <df3d/lib/JsonUtils.h>

namespace df3d {

static unique_ptr<Technique> ParseTechnique(const Json::Value &jsonTechnique)
{
    auto &rmgr = svc().resourceManager();

    std::string techName;
    jsonTechnique["id"] >> techName;

    DF3D_ASSERT_MESS(!techName.empty(), "Invalid technique name");

    auto technique = make_unique<Technique>(techName);

    const auto &jsonPasses = jsonTechnique["passes"];
    for (const auto &jsonPass : jsonPasses)
    {
        RenderPass pass;

        if (jsonPass.isMember("cull_face"))
        {
            auto cullFaceStr = jsonPass["cull_face"].asString();
            if (cullFaceStr == "NONE")
                pass.faceCullMode = FaceCullMode::NONE;
            else if (cullFaceStr == "BACK")
                pass.faceCullMode = FaceCullMode::BACK;
            else if (cullFaceStr == "FRONT")
                pass.faceCullMode = FaceCullMode::FRONT;
            else
                DFLOG_WARN("Unknown face cull mode %s", cullFaceStr.c_str());
        }
        if (jsonPass.isMember("depth_test"))
        {
            DF3D_ASSERT(jsonPass["depth_test"].isBool());
            pass.depthTest = jsonPass["depth_test"].asBool();
        }
        if (jsonPass.isMember("depth_write"))
        {
            DF3D_ASSERT(jsonPass["depth_write"].isBool());
            pass.depthWrite = jsonPass["depth_write"].asBool();
        }
        if (jsonPass.isMember("is_lit"))
        {
            DF3D_ASSERT(jsonPass["is_lit"].isBool());
            pass.lightingEnabled = jsonPass["is_lit"].asBool();
        }
        if (jsonPass.isMember("blend"))
        {
            auto blendModeStr = jsonPass["blend"].asString();
            if (blendModeStr == "NONE")
                pass.blendMode = BlendingMode::NONE;
            else if (blendModeStr == "ALPHA")
                pass.blendMode = BlendingMode::ALPHA;
            else if (blendModeStr == "ADDALPHA")
                pass.blendMode = BlendingMode::ADDALPHA;
            else if (blendModeStr == "ADD")
                pass.blendMode = BlendingMode::ADD;
            else
                DFLOG_WARN("Unknown blending mode %s", blendModeStr.c_str());

            pass.isTransparent = pass.blendMode != BlendingMode::NONE;
        }
        if (jsonPass.isMember("samplers"))
        {
            for (const auto &jsonSampler : jsonPass["samplers"])
            {
                auto samplerId = jsonSampler["id"].asString();
                DF3D_ASSERT(!samplerId.empty());
                auto textureResource = jsonSampler["path"].asString();

                if (auto texture = rmgr.getResource<TextureResource>(textureResource))
                    pass.setParam(samplerId, texture->handle);
                else
                    DFLOG_WARN("Sampler %s doesn't have a loaded texture %s", samplerId.c_str(), textureResource.c_str());
            }
        }
        if (jsonPass.isMember("shader_params"))
        {
            for (auto it = jsonPass["shader_params"].begin(); it != jsonPass["shader_params"].end(); ++it)
            {
                auto paramName = it.key().asString();
                if (it->isArray())
                {
                    glm::vec4 value;
                    (*it) >> value;
                    pass.setParam(paramName, value);
                }
                else
                {
                    float value;
                    (*it) >> value;
                    pass.setParam(paramName, value);
                }
            }
        }
        if (jsonPass.isMember("shader"))
        {
            auto shaderId = jsonPass["shader"].asString();
            if (shaderId != "colored")
            {
                if (auto program = rmgr.getResource<GpuProgramResource>(shaderId))
                    pass.program = program;
                else
                    DFLOG_WARN("Failed to set shader, resource %s not found", shaderId.c_str());
            }
            else
                pass.program = svc().renderManager().getEmbedResources().coloredProgram;
        }

        technique->passes.push_back(pass);
    }

    return technique;
}

static void PreloadTechniqueData(const Json::Value &root, std::vector<ResourceID> &outDeps)
{
    const auto &jsonPasses = root["passes"];
    for (const auto &jsonPass : jsonPasses)
    {
        if (jsonPass.isMember("samplers"))
        {
            const auto &samplersJson = jsonPass["samplers"];
            for (const auto &sampler : samplersJson)
            {
                DF3D_ASSERT(sampler.isMember("path"));
                outDeps.push_back(sampler["path"].asString());
            }
        }

        if (jsonPass.isMember("shader"))
        {
            DF3D_ASSERT(jsonPass["shader"].isString());

            auto shaderId = jsonPass["shader"].asString();
            // TODO: remove embed resources.
            if (shaderId != "colored")
                outDeps.push_back(shaderId);
        }
    }
}

void MaterialLibResource::parse(const Json::Value &root)
{
    const auto &jsonMaterials = root["materials"];
    for (const auto &jsonMaterial : jsonMaterials)
    {
        auto id = jsonMaterial["id"].asString();
        DF3D_ASSERT(!utils::contains_key(m_materials, id));

        Material material;

        const auto &jsonTechniques = jsonMaterial["techniques"];

        DF3D_ASSERT(!jsonTechniques.empty());

        // FIXME: this is a workaround.
        auto foundPrefTech = std::find_if(jsonTechniques.begin(), jsonTechniques.end(), [](const Json::Value &node) {
            return node["id"].asString() == PREFERRED_TECHNIQUE;
        });

        unique_ptr<Technique> technique;

        if (foundPrefTech != jsonTechniques.end())
        {
            technique = ParseTechnique(*foundPrefTech);
        }
        else
        {
            technique = ParseTechnique(*jsonTechniques.begin());
        }

        material.addTechnique(*technique);
        material.setCurrentTechnique(technique->name);
        material.setName(id);

        m_materials[id] = material;
    }
}

MaterialLibResource::MaterialLibResource(const Json::Value &root)
{
    parse(root);
}

const Material* MaterialLibResource::getMaterial(const std::string &name) const
{
    auto found = m_materials.find(name);
    if (found != m_materials.end())
        return &found->second;
    return nullptr;
}

void MaterialLibHolder::listDependencies(ResourceDataSource &dataSource, std::vector<ResourceID> &outDeps)
{
    auto root = JsonUtils::fromFile(dataSource);
    if (root.isNull())
        return;

    const auto &jsonMaterials = root["materials"];
    for (const auto &jsonMaterial : jsonMaterials)
    {
        const auto &jsonTechniques = jsonMaterial["techniques"];

        DF3D_ASSERT(!jsonTechniques.empty());

        // FIXME: this is a workaround.
        auto foundPrefTech = std::find_if(jsonTechniques.begin(), jsonTechniques.end(), [](const Json::Value &node) {
            return node["id"].asString() == PREFERRED_TECHNIQUE;
        });

        if (foundPrefTech != jsonTechniques.end())
        {
            PreloadTechniqueData(*foundPrefTech, outDeps);
        }
        else
        {
            PreloadTechniqueData(*jsonTechniques.begin(), outDeps);
        }
    }
}

bool MaterialLibHolder::decodeStartup(ResourceDataSource &dataSource, Allocator &allocator)
{
    auto root = JsonUtils::fromFile(dataSource);
    if (root.isNull())
        return false;

    m_root = MAKE_NEW(allocator, Json::Value)(std::move(root));

    return true;
}

void MaterialLibHolder::decodeCleanup(Allocator &allocator)
{
    MAKE_DELETE(allocator, m_root);
    m_root = nullptr;
}

bool MaterialLibHolder::createResource(Allocator &allocator)
{
    m_resource = MAKE_NEW(allocator, MaterialLibResource)(*m_root);

    return true;
}

void MaterialLibHolder::destroyResource(Allocator &allocator)
{
    MAKE_DELETE(allocator, m_resource);
    m_resource = nullptr;
}

}
