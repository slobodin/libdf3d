#include "MaterialLibLoaders.h"

#include <df3d/engine/EngineController.h>
#include <df3d/engine/resources/ResourceFactory.h>
#include <df3d/engine/resources/ResourceManager.h>
#include <df3d/engine/io/DataSource.h>
#include <df3d/engine/render/MaterialLib.h>
#include <df3d/engine/render/Material.h>
#include <df3d/engine/render/Technique.h>
#include <df3d/engine/render/RenderPass.h>
#include <df3d/engine/render/GpuProgram.h>
#include <df3d/engine/render/Texture.h>
#include <df3d/engine/render/IRenderBackend.h>
#include <df3d/lib/Utils.h>
#include <df3d/lib/JsonUtils.h>

namespace df3d {

static std::map<std::string, FaceCullMode> FaceCullModeValues =
{
    { "NONE", FaceCullMode::NONE },
    { "BACK", FaceCullMode::BACK },
    { "FRONT", FaceCullMode::FRONT }
};

static std::map<std::string, BlendingMode> BlendModeValues =
{
    { "NONE", BlendingMode::NONE },
    { "ALPHA", BlendingMode::ALPHA },
    { "ADDALPHA", BlendingMode::ADDALPHA }
};

template<typename V>
static void SetPassParam(const std::string &param, const std::string &valueStr, const std::map<std::string, V> &acceptedValues, std::function<void(V)> setter)
{
    if (valueStr.empty())
    {
        DFLOG_WARN("Empty pass param value");
        return;
    }

    auto found = acceptedValues.find(valueStr);
    if (found != acceptedValues.end())
        setter(found->second);
    else
    {
        // Print warning and list of accepted values for this parameter.
        std::string acceptedString;
        for (auto &it : acceptedValues)
            acceptedString += it.first + ", ";

        acceptedString.pop_back();
        acceptedString.pop_back();

        DFLOG_WARN("Invalid pass parameter %s found while parsing. Accepted values are: %s", param.c_str(), acceptedString.c_str());
    }
}

static uint32_t GetTextureFlags(const Json::Value &node)
{
    // FIXME: default max anisotropy
    uint32_t retRes = TEXTURE_MAX_ANISOTROPY;

    if (node.isMember("filtering"))
    {
        auto valueStr = node["filtering"].asString();
        if (valueStr == "NEAREST")
            retRes |= TEXTURE_FILTERING_NEAREST;
        else if (valueStr == "BILINEAR")
            retRes |= TEXTURE_FILTERING_BILINEAR;
        else if (valueStr == "TRILINEAR")
            retRes |= TEXTURE_FILTERING_TRILINEAR;
        else
            DFLOG_WARN("Unknown filtering mode %s", valueStr.c_str());
    }
    else
        retRes |= TEXTURE_FILTERING_TRILINEAR;

    if (node.isMember("wrap_mode"))
    {
        auto valueStr = node["wrap_mode"].asString();
        if (valueStr == "WRAP")
            retRes |= TEXTURE_WRAP_MODE_REPEAT;
        else if (valueStr == "CLAMP")
            retRes |= TEXTURE_WRAP_MODE_CLAMP;
        else
            DFLOG_WARN("Unknown wrap_mode mode %s", valueStr.c_str());
    }
    else
        retRes |= TEXTURE_WRAP_MODE_REPEAT;

    return retRes;
}

shared_ptr<GpuProgram> ParseShaderNode(const Json::Value &node)
{
    // First, check for embed program usage.
    if (node.isMember("embed"))
    {
        auto programName = node["embed"].asString();
        if (programName == "colored")
            return svc().resourceManager().getFactory().createColoredGpuProgram();
        else if (programName == "simple_lighting")
            return svc().resourceManager().getFactory().createSimpleLightingGpuProgram();
        else
        {
            DFLOG_WARN("Invalid embed program name %s", programName.c_str());
            return nullptr;
        }
    }

    // Otherwise, create program from passed paths.
    auto vshader = node["vertex"].asString();
    auto fshader = node["fragment"].asString();

    if (vshader.empty() || fshader.empty())
    {
        DFLOG_WARN("Invalid shader properties");
        return nullptr;
    }

    return svc().resourceManager().getFactory().createGpuProgram(vshader, fshader);
}

void ParseShaderParamsNode(const Json::Value &node, shared_ptr<RenderPass> pass)
{
    for (auto it = node.begin(); it != node.end(); it++)
    {
        auto key = it.key().asString();
        auto value = it->asFloat();
        pass->getPassParam(key)->setValue(value);
    }
}

shared_ptr<Texture> ParseSamplerNode(const Json::Value &node)
{
    if (node.isMember("type"))
    {
        auto typeStr = node["type"].asString();

        if (typeStr == "TEXTURE_2D")
        {
            auto path = node["path"].asString();

            return svc().resourceManager().getFactory().createTexture(path, GetTextureFlags(node), ResourceLoadingMode::ASYNC);
        }
        else if (typeStr == "TEXTURE_CUBE")
        {
            DF3D_ASSERT_MESS(false, "TEXTURE_CUBE is not supported");
            return nullptr;

            //return svc().resourceManager().getFactory().createCubeTexture(path, GetTextureFlags(node), ResourceLoadingMode::ASYNC);
        }

        DFLOG_WARN("Unknown texture type %s", typeStr.c_str());
        return nullptr;

        // TODO: other types are:
        // TEXTURE_1D TEXTURE_2D TEXTURE_3D
    }
    else
    {
        // Assuming using empty white texture.
        // FIXME:
        return nullptr;
    }
}

static shared_ptr<RenderPass> ParsePassNode(const Json::Value &node)
{
    auto pass = make_shared<RenderPass>();

    // Get material params.
    if (node.isMember("diffuse"))
    {
        glm::vec4 color;
        node["diffuse"] >> color;
        pass->getPassParam("material_diffuse")->setValue(color);
    }
    if (node.isMember("specular"))
    {
        glm::vec4 color;
        node["specular"] >> color;
        pass->getPassParam("material_specular")->setValue(color);
    }
    if (node.isMember("shininess"))
    {
        float shininess;
        node["shininess"] >> shininess;
        pass->getPassParam("material_shininess")->setValue(shininess);
    }
    if (node.isMember("cull_face"))
    {
        std::function<void(FaceCullMode)> fn = std::bind(&RenderPass::setFaceCullMode, pass.get(), std::placeholders::_1);
        SetPassParam("cull_face", node["cull_face"].asString(), FaceCullModeValues, fn);
    }
    if (node.isMember("depth_test"))
    {
        bool depthTest;
        node["depth_test"] >> depthTest;
        pass->enableDepthTest(depthTest);
    }
    if (node.isMember("depth_write"))
    {
        bool depthWrite;
        node["depth_write"] >> depthWrite;
        pass->enableDepthWrite(depthWrite);
    }
    if (node.isMember("is_lit"))
    {
        bool isLit;
        node["is_lit"] >> isLit;
        pass->enableLighting(isLit);
    }
    if (node.isMember("blend"))
    {
        std::function<void(BlendingMode)> fn = std::bind(&RenderPass::setBlendMode, pass.get(), std::placeholders::_1);
        SetPassParam("blend", node["blend"].asString(), BlendModeValues, fn);
    }

    // Set other pass params before setting a gpu program.
    if (node.isMember("samplers"))
    {
        for (const auto &jsonSampler : node["samplers"])
        {
            auto samplerId = jsonSampler["id"].asString();
            if (samplerId.empty())
            {
                DFLOG_WARN("Empty sampler id!");
                continue;
            }

            pass->getPassParam(samplerId)->setValue(ParseSamplerNode(jsonSampler));
        }
    }
    if (node.isMember("shader_params"))
    {
        ParseShaderParamsNode(node["shader_params"], pass);
    }

    // Get material shader.
    if (node.isMember("shader"))
    {
        auto gpuProgram = ParseShaderNode(node["shader"]);
        pass->setGpuProgram(gpuProgram);
    }

    return pass;
}

static shared_ptr<Technique> ParseTechniqueNode(const Json::Value &node)
{
    std::string techName;
    node["id"] >> techName;

    if (techName.empty())
    {
        DFLOG_WARN("Invalid technique name found");
        return nullptr;
    }

    auto technique = make_shared<Technique>(techName);

    const auto &jsonPasses = node["passes"];

    for (const auto &jsonPass : jsonPasses)
    {
        auto pass = ParsePassNode(jsonPass);
        if (pass)
            technique->appendPass(*pass);
    }

    return technique;
}

static shared_ptr<Material> ParseMaterialNode(const Json::Value &node)
{
    std::string materialName;
    node["id"] >> materialName;

    if (materialName.empty())
    {
        DFLOG_WARN("Invalid material name");
        return nullptr;
    }

    auto material = make_shared<Material>(materialName);

    const auto &jsonTechniques = node["techniques"];

    // FIXME: this is a workaround.
    auto foundPrefTech = std::find_if(jsonTechniques.begin(), jsonTechniques.end(), [](const Json::Value &node) {
        return node["id"].asString() == MaterialLib::PREFERRED_TECHNIQUE;
    });

    if (foundPrefTech != jsonTechniques.end())
    {
        auto technique = ParseTechniqueNode(*foundPrefTech);
        if (technique)
        {
            material->appendTechnique(*technique);
            material->setCurrentTechnique(technique->getName());
        }
    }
    else
    {
        for (const auto &jsonTech : jsonTechniques)
        {
            auto technique = ParseTechniqueNode(jsonTech);
            if (!technique)
                continue;

            material->appendTechnique(*technique);
            material->setCurrentTechnique(technique->getName());

            // FIXME: workaround, using just the first technique.
            break;
        }
    }

    return material;
}

static bool ParseMaterialLib(const Json::Value &jsonRoot, std::vector<shared_ptr<Material>> &outMaterials)
{
    const auto &jsonMaterials = jsonRoot["materials"];
    for (const auto &jsonMaterial : jsonMaterials)
    {
        auto material = ParseMaterialNode(jsonMaterial);
        if (material)
            outMaterials.push_back(material);
    }

    return true;
}

MaterialLibFSLoader::MaterialLibFSLoader(const std::string &path)
    : FSResourceLoader(ResourceLoadingMode::IMMEDIATE),
    m_path(path)
{

}

MaterialLib* MaterialLibFSLoader::createDummy()
{
    return new MaterialLib();
}

bool MaterialLibFSLoader::decode(shared_ptr<DataSource> source)
{
    std::string filedata(source->getSize(), 0);
    source->read(&filedata[0], filedata.size());

    m_root = JsonUtils::fromSource(filedata);
    if (m_root.isNull())
    {
        DFLOG_WARN("Failed to parse JSON in material lib '%s'", source->getPath().c_str());
        return false;
    }

    return true;
}

void MaterialLibFSLoader::onDecoded(Resource *resource)
{
    auto mtlLib = static_cast<MaterialLib*>(resource);

    std::vector<shared_ptr<Material>> materials;

    if (ParseMaterialLib(m_root, materials))
    {
        for (const auto &material : materials)
            mtlLib->appendMaterial(material);
    }
    else
    {
        DFLOG_WARN("Failed to properly init material lib '%s'", resource->getFilePath().c_str());
    }
}

}
