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

    DF3D_ASSERT(jsonTechnique.isMember("id"));

    auto technique = make_unique<Technique>(Id(jsonTechnique["id"].asCString()));

    for (const auto &jsonPass : jsonTechnique["passes"])
    {
        RenderPass pass;

        bool notTransparent = false;

        for (auto it = jsonPass.begin(); it != jsonPass.end(); ++it)
        {
            auto paramName = Id(it.key().asCString());
            if (paramName == Id("cull_face"))
            {
                pass.state &= ~RENDER_STATE_FACE_CULL_MASK;
                auto cullFaceValue = Id(it->asCString());
                if (cullFaceValue == Id("BACK"))
                    pass.state |= RENDER_STATE_FRONT_FACE_CCW;
            }
            else if (paramName == Id("depth_test"))
            {
                DF3D_ASSERT(it->isBool());
                pass.setDepthTest(it->asBool());
            }
            else if (paramName == Id("depth_write"))
            {
                DF3D_ASSERT(it->isBool());
                pass.setDepthWrite(it->asBool());
            }
            else if (paramName == Id("not_transparent"))
            {
                DF3D_ASSERT(it->isBool());
                notTransparent = it->asBool();
            }
            else if (paramName == Id("is_lit"))
            {
                DF3D_ASSERT(it->isBool());
                if (it->asBool())
                    pass.preferredBucket = RQ_BUCKET_LIT;
            }
            else if (paramName == Id("blend"))
            {
                auto blendModeValue = Id(it->asCString());
                if (blendModeValue == Id("ALPHA"))
                {
                    pass.setBlending(Blending::ALPHA);
                    pass.preferredBucket = RQ_BUCKET_TRANSPARENT;
                }
                else if (blendModeValue == Id("ADDALPHA"))
                {
                    pass.setBlending(Blending::ADDALPHA);
                    pass.preferredBucket = RQ_BUCKET_TRANSPARENT;
                }
                else if (blendModeValue == Id("ADD"))
                {
                    pass.setBlending(Blending::ADD);
                    pass.preferredBucket = RQ_BUCKET_TRANSPARENT;
                }
                else
                    DFLOG_WARN("Unknown blending mode %s", it->asCString());
            }
            else if (paramName == Id("samplers"))
            {
                for (const auto &jsonSampler : *it)
                {
                    DF3D_ASSERT(jsonSampler.isMember("id"));
                    DF3D_ASSERT(jsonSampler.isMember("path"));

                    auto samplerId = jsonSampler["id"].asCString();
                    auto textureResourcePath = jsonSampler["path"].asCString();

                    if (auto texture = rmgr.getResource<TextureResource>(Id(textureResourcePath)))
                        pass.setParam(Id(samplerId), texture->handle);
                    else
                        DFLOG_WARN("Sampler %s doesn't have a loaded texture %s", samplerId, textureResourcePath);
                }
            }
            else if (paramName == Id("shader_params"))
            {
                for (auto shaderIt = it->begin(); shaderIt != it->end(); ++shaderIt)
                {
                    auto paramName = Id(shaderIt.key().asCString());
                    if (shaderIt->isArray())
                    {
                        const auto &arr = *shaderIt;

                        glm::vec4 value{ arr[0].asFloat(), arr[1].asFloat(), arr[2].asFloat(), arr[3].asFloat() };

                        pass.setParam(paramName, value);
                    }
                    else
                    {
                        DF3D_ASSERT(shaderIt->isNumeric());

                        pass.setParam(paramName, shaderIt->asFloat());
                    }
                }
            }
            else if (paramName == Id("shader"))
            {
                auto shaderPath = it->asCString();
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

        if (notTransparent)
        {
            pass.preferredBucket = RQ_BUCKET_NOT_LIT;
        }

        technique->passes.push_back(pass);
    }

    return technique;
}

static void PreloadTechniqueData(const Json::Value &root, std::vector<std::string> &outDeps)
{
    for (const auto &jsonPass : root["passes"])
    {
        if (jsonPass.isMember("samplers"))
        {
            for (const auto &sampler : jsonPass["samplers"])
            {
                DF3D_ASSERT(sampler.isMember("path"));
                outDeps.push_back(sampler["path"].asString());
            }
        }

        if (jsonPass.isMember("shader"))
        {
            std::string shaderId = jsonPass["shader"].asString();
            // TODO: remove embed resources.
            if (shaderId != "colored")
                outDeps.push_back(shaderId);
        }
    }
}

void MaterialLibResource::parse(const Json::Value &root)
{
    for (const auto &jsonMaterial : root["materials"])
    {
        DF3D_ASSERT(jsonMaterial.isMember("id"));

        auto id = Id(jsonMaterial["id"].asCString());
        DF3D_ASSERT(!utils::contains_key(m_materials, id));

        Material material;

        const auto &jsonTechniques = jsonMaterial["techniques"];

        DF3D_ASSERT(!jsonTechniques.empty());

        auto technique = ParseTechnique(*jsonTechniques.begin());

        material.addTechnique(*technique);
        material.setCurrentTechnique(technique->name);
        material.setName(jsonMaterial["id"].asString());

        m_materials[id] = material;
    }
}

MaterialLibResource::MaterialLibResource(const Json::Value &root)
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
    if (root.isNull())
        return;

    for (const auto &jsonMaterial : root["materials"])
    {
        const auto &jsonTechniques = jsonMaterial["techniques"];

        DF3D_ASSERT(!jsonTechniques.empty());

        PreloadTechniqueData(*jsonTechniques.begin(), outDeps);
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
