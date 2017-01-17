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

static unique_ptr<Technique> ParseTechnique(const rapidjson::Value &jsonTechnique)
{
    auto &rmgr = svc().resourceManager();

    DF3D_ASSERT(jsonTechnique.HasMember("id"));

    auto technique = make_unique<Technique>(Id(jsonTechnique["id"].GetString()));

    auto jsonPasses = jsonTechnique["passes"].GetArray();
    for (const auto &jsonPass : jsonPasses)
    {
        RenderPass pass;

        for (const auto &kv : jsonPass.GetObject())
        {
            auto paramName = Id(kv.name.GetString());
            if (paramName == Id("cull_face"))
            {
                auto cullFaceValue = Id(kv.value.GetString());
                if (cullFaceValue == Id("NONE"))
                    pass.faceCullMode = FaceCullMode::NONE;
                else if (cullFaceValue == Id("BACK"))
                    pass.faceCullMode = FaceCullMode::BACK;
                else if (cullFaceValue == Id("FRONT"))
                    pass.faceCullMode = FaceCullMode::FRONT;
                else
                    DFLOG_WARN("Unknown face cull mode %s", kv.value.GetString());
            }
            else if (paramName == Id("depth_test"))
            {
                DF3D_ASSERT(kv.value.IsBool());
                pass.depthTest = kv.value.GetBool();
            }
            else if (paramName == Id("depth_write"))
            {
                DF3D_ASSERT(kv.value.IsBool());
                pass.depthWrite = kv.value.GetBool();
            }
            else if (paramName == Id("is_lit"))
            {
                DF3D_ASSERT(kv.value.IsBool());
                pass.lightingEnabled = kv.value.GetBool();
            }
            else if (paramName == Id("blend"))
            {
                auto blendModeValue = Id(kv.value.GetString());
                if (blendModeValue == Id("NONE"))
                    pass.blendMode = BlendingMode::NONE;
                else if (blendModeValue == Id("ALPHA"))
                    pass.blendMode = BlendingMode::ALPHA;
                else if (blendModeValue == Id("ADDALPHA"))
                    pass.blendMode = BlendingMode::ADDALPHA;
                else if (blendModeValue == Id("ADD"))
                    pass.blendMode = BlendingMode::ADD;
                else
                    DFLOG_WARN("Unknown blending mode %s", kv.value.GetString());

                pass.isTransparent = pass.blendMode != BlendingMode::NONE;
            }
            else if (paramName == Id("samplers"))
            {
                auto samplersJson = kv.value.GetArray();
                for (const auto &jsonSampler : samplersJson)
                {
                    DF3D_ASSERT(jsonSampler.HasMember("id"));
                    DF3D_ASSERT(jsonSampler.HasMember("path"));

                    auto samplerId = jsonSampler["id"].GetString();
                    auto textureResourcePath = jsonSampler["path"].GetString();

                    if (auto texture = rmgr.getResource<TextureResource>(Id(textureResourcePath)))
                        pass.setParam(Id(samplerId), texture->handle);
                    else
                        DFLOG_WARN("Sampler %s doesn't have a loaded texture %s", samplerId, textureResourcePath);
                }
            }
            else if (paramName == Id("shader_params"))
            {
                for (const auto &it : kv.value.GetObject())
                {
                    auto paramName = Id(it.name.GetString());
                    if (it.value.IsArray())
                    {
                        auto arr = it.value.GetArray();

                        glm::vec4 value{ arr[0].GetFloat(), arr[1].GetFloat(), arr[2].GetFloat(), arr[3].GetFloat() };

                        pass.setParam(paramName, value);
                    }
                    else
                    {
                        DF3D_ASSERT(it.value.IsNumber());

                        pass.setParam(paramName, it.value.GetFloat());
                    }
                }
            }
            else if (paramName == Id("shader"))
            {
                auto shaderPath = jsonPass["shader"].GetString();
                if (strcmp(shaderPath, "colored") == 0)
                {
                    pass.program = svc().renderManager().getEmbedResources().coloredProgram;
                }
                else
                {
                    if (auto program = rmgr.getResource<GpuProgramResource>(Id(shaderPath)))
                        pass.program = program;
                    else
                        DFLOG_WARN("Failed to set shader, resource %s not found", shaderPath);
                }
            }
        }

        technique->passes.push_back(pass);
    }

    return technique;
}

static void PreloadTechniqueData(const rapidjson::Value &root, std::vector<std::string> &outDeps)
{
    auto jsonPasses = root["passes"].GetArray();

    for (const auto &jsonPass : jsonPasses)
    {
        auto foundSamplersJson = jsonPass.FindMember("samplers");
        if (foundSamplersJson != jsonPass.MemberEnd())
        {
            for (const auto &sampler : foundSamplersJson->value.GetArray())
            {
                DF3D_ASSERT(sampler.HasMember("path"));
                outDeps.push_back(std::string(sampler["path"].GetString()));
            }
        }

        auto foundShaderJson = jsonPass.FindMember("shader");
        if (foundShaderJson != jsonPass.MemberEnd())
        {
            DF3D_ASSERT(foundShaderJson->value.IsString());

            std::string shaderId = foundShaderJson->value.GetString();
            // TODO: remove embed resources.
            if (shaderId != "colored")
                outDeps.push_back(shaderId);
        }
    }
}

void MaterialLibResource::parse(const rapidjson::Value &root)
{
    auto jsonMaterials = root["materials"].GetArray();
    for (const auto &jsonMaterial : jsonMaterials)
    {
        DF3D_ASSERT(jsonMaterial.HasMember("id"));

        auto id = Id(jsonMaterial["id"].GetString());
        DF3D_ASSERT(!utils::contains_key(m_materials, id));

        Material material;

        auto jsonTechniques = jsonMaterial["techniques"].GetArray();

        DF3D_ASSERT(jsonTechniques.Size() > 0);

        // FIXME: this is a workaround.
        auto foundPrefTech = std::find_if(jsonTechniques.begin(), jsonTechniques.end(), [](const rapidjson::Value &node) {
            return Id(node["id"].GetString()) == PREFERRED_TECHNIQUE;
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
        material.setName(jsonMaterial["id"].GetString());

        m_materials[id] = material;
    }
}

MaterialLibResource::MaterialLibResource(const rapidjson::Value &root)
{
    parse(root);
}

const Material* MaterialLibResource::getMaterial(Id name) const
{
    auto found = m_materials.find(name);
    if (found != m_materials.end())
        return &found->second;
    return nullptr;
}

void MaterialLibHolder::listDependencies(ResourceDataSource &dataSource, std::vector<std::string> &outDeps)
{
    auto root = JsonUtils::fromFile(dataSource);
    if (root.IsNull())
        return;

    auto jsonMaterials = root["materials"].GetArray();
    for (const auto &jsonMaterial : jsonMaterials)
    {
        auto jsonTechniques = jsonMaterial["techniques"].GetArray();

        DF3D_ASSERT(jsonTechniques.Size() > 0);

        // FIXME: this is a workaround.
        auto foundPrefTech = std::find_if(jsonTechniques.begin(), jsonTechniques.end(), [](const rapidjson::Value &node) {
            return Id(node["id"].GetString()) == PREFERRED_TECHNIQUE;
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
    if (root.IsNull())
        return false;

    m_root = MAKE_NEW(allocator, rapidjson::Document)(std::move(root));

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
