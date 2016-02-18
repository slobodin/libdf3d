#include "MaterialLibLoaders.h"

#include <libdf3d/base/EngineController.h>
#include <libdf3d/resources/ResourceFactory.h>
#include <libdf3d/resources/ResourceManager.h>
#include <libdf3d/io/FileDataSource.h>
#include <libdf3d/render/MaterialLib.h>
#include <libdf3d/render/Material.h>
#include <libdf3d/render/Technique.h>
#include <libdf3d/render/RenderPass.h>
#include <libdf3d/render/GpuProgram.h>
#include <libdf3d/render/Texture2D.h>
#include <libdf3d/render/TextureCube.h>
#include <libdf3d/utils/Utils.h>

namespace df3d {

std::map<std::string, RenderPass::WindingOrder> woValues =
{
    { "CW", RenderPass::WindingOrder::CW },
    { "CCW", RenderPass::WindingOrder::CCW }
};

std::map<std::string, RenderPass::PolygonMode> polygonModeValues =
{
    { "FILL", RenderPass::PolygonMode::FILL },
    { "WIRE", RenderPass::PolygonMode::WIRE }
};

std::map<std::string, RenderPass::FaceCullMode> faceCullModeValues =
{
    { "NONE", RenderPass::FaceCullMode::NONE },
    { "BACK", RenderPass::FaceCullMode::BACK },
    { "FRONT", RenderPass::FaceCullMode::FRONT },
    { "FRONT_AND_BACK", RenderPass::FaceCullMode::FRONT_AND_BACK }
};

std::map<std::string, TextureFiltering> textureFilteringValues =
{
    { "NEAREST", TextureFiltering::NEAREST },
    { "BILINEAR", TextureFiltering::BILINEAR },
    { "TRILINEAR", TextureFiltering::TRILINEAR }
};

std::map<std::string, TextureWrapMode> textureWrapValues =
{
    { "WRAP", TextureWrapMode::WRAP },
    { "CLAMP", TextureWrapMode::CLAMP }
};

std::map<std::string, RenderPass::BlendingMode> blendModeValues =
{
    { "NONE", RenderPass::BlendingMode::NONE },
    { "ALPHA", RenderPass::BlendingMode::ALPHA },
    { "ADDALPHA", RenderPass::BlendingMode::ADDALPHA }
};

std::map<std::string, bool> boolValues =
{
    { "true", true },
    { "false", false },
    { "1", true },
    { "0", false }
};

template<typename V>
void setPassParam(const std::string &param, const std::string &valueStr, const std::map<std::string, V> &acceptedValues, std::function<void(V)> setter, const std::string &libName)
{
    if (valueStr.empty())
        return;

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

        glog << "Invalid pass parameter" << param << "found while parsing" << libName << ". Accepted values are :" << acceptedString << logwarn;
    }
}

static TextureCreationParams getTextureCreationParams(const std::map<std::string, std::string> &keyValues, const std::string &libName)
{
    TextureCreationParams retRes;

    for (const auto &keyval : keyValues)
    {
        if (keyval.first == "filtering")
        {
            std::function<void(TextureFiltering)> fn = std::bind(&TextureCreationParams::setFiltering, &retRes, std::placeholders::_1);
            setPassParam(keyval.first, keyval.second, textureFilteringValues, fn, libName);
        }
        else if (keyval.first == "wrap_mode")
        {
            std::function<void(TextureWrapMode)> fn = std::bind(&TextureCreationParams::setWrapMode, &retRes, std::placeholders::_1);
            setPassParam(keyval.first, keyval.second, textureWrapValues, fn, libName);
        }
        else if (keyval.first == "mipmaps")
        {
            std::function<void(bool)> fn = std::bind(&TextureCreationParams::setMipmapped, &retRes, std::placeholders::_1);
            setPassParam(keyval.first, keyval.second, boolValues, fn, libName);
        }
        else if (keyval.first == "anisotropy")
        {
            if (keyval.second == "max")
                retRes.setAnisotropyLevel(ANISOTROPY_LEVEL_MAX);
            else
                retRes.setAnisotropyLevel(utils::from_string<int>(keyval.second));
        }
    }

    return retRes;
}

const std::string MATERIAL_TYPE = "material";
const std::string TECHNIQUE_TYPE = "technique";
const std::string PASS_TYPE = "pass";
const std::string SHADER_TYPE = "shader";
const std::string SAMPLER_TYPE = "sampler";
const std::string SHADER_PARAMS_TYPE = "shader_params";

const std::string NodesTypes[] = {
    MATERIAL_TYPE,
    TECHNIQUE_TYPE,
    PASS_TYPE,
    SHADER_TYPE,
    SAMPLER_TYPE,
    SHADER_PARAMS_TYPE
};

struct MaterialLibNode
{
private:
    static bool isKeyWord(const std::string &name)
    {
        return utils::contains(NodesTypes, name);
    }

public:
    std::string type;
    std::string name;
    std::map<std::string, std::string> keyValues;

    std::vector<MaterialLibNode *> children;
    MaterialLibNode(const std::string &nodeType)
        : type(nodeType)
    {
    }

    static MaterialLibNode *createTree(std::istringstream &is)
    {
        std::stack<MaterialLibNode *> nodesStack;

        nodesStack.push(new MaterialLibNode("__root"));

        std::string tok;
        bool skippingLines = false;
        while (is >> tok)
        {
            if (utils::starts_with(tok, "#endif"))
            {
                utils::skip_line(is);
                skippingLines = false;
                continue;
            }

            if (skippingLines)
            {
                utils::skip_line(is);
                continue;
            }

            // Skip empty lines.
            utils::trim_left(tok);
            if (tok.empty())
                continue;

            // Skip comment lines.
            if (utils::starts_with(tok, "//"))
            {
                utils::skip_line(is);
                continue;
            }

            if (utils::starts_with(tok, "#ifdef"))
            {
                std::string define;
                is >> define;

                skippingLines = !utils::contains(MaterialLib::Defines, define);
                continue;
            }

            if (tok == "}")
            {
                auto child = nodesStack.top();

                if (nodesStack.size() < 2)
                    return nullptr;

                nodesStack.pop();

                auto last = nodesStack.top();
                last->children.push_back(child);
                continue;
            }

            if (isKeyWord(tok))
            {
                nodesStack.push(new MaterialLibNode(tok));

                // Try to get name.
                std::string name;
                is >> name;
                if (name != "{")
                {
                    nodesStack.top()->name = name;
                    // Skip '{'
                    is >> name;
                }

                continue;
            }

            // Must be a key value pair in this case.
            auto last = nodesStack.top();
            std::string value;
            std::getline(is, value);

            utils::trim(value);

            last->keyValues[tok] = value;
        }

        return nodesStack.top();
    }

    static void clean(MaterialLibNode *&node)
    {
        for (auto &c : node->children)
        {
            clean(c);
        }

        delete node;
    }
};

class MaterialLibParser
{
    std::string m_libPath;    // For logging.

    shared_ptr<Material> parseMaterialNode(const MaterialLibNode &node)
    {
        if (node.name.empty())
        {
            glog << "Invalid material name found while parsing" << m_libPath << logwarn;
            return nullptr;
        }

        auto material = make_shared<Material>(node.name);

        std::string defaultTechniqueName;
        for (const auto &n : node.children)
        {
            if (n->type == TECHNIQUE_TYPE)
            {
                auto technique = parseTechniqueNode(*n);
                if (!technique)
                    continue;

                // Set first technique as default.
                if (defaultTechniqueName.empty())
                    defaultTechniqueName = technique->getName();

                material->appendTechnique(*technique);
            }
        }

        material->setCurrentTechnique(defaultTechniqueName);

        return material;
    }

    shared_ptr<Technique> parseTechniqueNode(const MaterialLibNode &node)
    {
        if (node.name.empty())
        {
            glog << "Invalid technique name found while parsing" << m_libPath << logwarn;
            return nullptr;
        }

        auto technique = make_shared<Technique>(node.name);

        for (const auto &n : node.children)
        {
            if (n->type == PASS_TYPE)
            {
                auto pass = parsePassNode(*n);
                if (pass)
                    technique->appendPass(*pass);
            }
        }

        return technique;
    }

    shared_ptr<RenderPass> parsePassNode(const MaterialLibNode &node)
    {
        auto pass = shared_ptr<RenderPass>(new RenderPass(node.name));

        // Get material params.
        for (auto &keyval : node.keyValues)
        {
            std::stringstream val(keyval.second);
            if (keyval.first == "ambient")
            {
                auto color = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);
                val >> color.r >> color.g >> color.b >> color.a;
                pass->setAmbientColor(color);
            }
            else if (keyval.first == "diffuse")
            {
                auto color = glm::vec4(0.8f, 0.8f, 0.8f, 1.0f);
                val >> color.r >> color.g >> color.b >> color.a;
                pass->setDiffuseColor(color);
            }
            else if (keyval.first == "specular")
            {
                auto color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
                val >> color.r >> color.g >> color.b >> color.a;
                pass->setSpecularColor(color);
            }
            else if (keyval.first == "emissive")
            {
                auto color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
                val >> color.r >> color.g >> color.b >> color.a;
                pass->setEmissiveColor(color);
            }
            else if (keyval.first == "shininess")
            {
                float shininess = 64.0f;
                val >> shininess;
                pass->setShininess(shininess);
            }
            else if (keyval.first == "cull_face")
            {
                std::function<void(RenderPass::FaceCullMode)> fn = std::bind(&RenderPass::setFaceCullMode, pass.get(), std::placeholders::_1);
                setPassParam(keyval.first, keyval.second, faceCullModeValues, fn, m_libPath);
            }
            else if (keyval.first == "front_face")
            {
                std::function<void(RenderPass::WindingOrder)> fn = std::bind(&RenderPass::setFrontFaceWinding, pass.get(), std::placeholders::_1);
                setPassParam(keyval.first, keyval.second, woValues, fn, m_libPath);
            }
            else if (keyval.first == "polygon_mode")
            {
                std::function<void(RenderPass::PolygonMode)> fn = std::bind(&RenderPass::setPolygonDrawMode, pass.get(), std::placeholders::_1);
                setPassParam(keyval.first, keyval.second, polygonModeValues, fn, m_libPath);
            }
            else if (keyval.first == "depth_test")
            {
                std::function<void(bool)> fn = std::bind(&RenderPass::enableDepthTest, pass.get(), std::placeholders::_1);
                setPassParam(keyval.first, keyval.second, boolValues, fn, m_libPath);
            }
            else if (keyval.first == "depth_write")
            {
                std::function<void(bool)> fn = std::bind(&RenderPass::enableDepthWrite, pass.get(), std::placeholders::_1);
                setPassParam(keyval.first, keyval.second, boolValues, fn, m_libPath);
            }
            else if (keyval.first == "is_lit")
            {
                std::function<void(bool)> fn = std::bind(&RenderPass::enableLighting, pass.get(), std::placeholders::_1);
                setPassParam(keyval.first, keyval.second, boolValues, fn, m_libPath);
            }
            else if (keyval.first == "blend")
            {
                std::function<void(RenderPass::BlendingMode)> fn = std::bind(&RenderPass::setBlendMode, pass.get(), std::placeholders::_1);
                setPassParam(keyval.first, keyval.second, blendModeValues, fn, m_libPath);
            }
        }

        // Get material shader and sampler params.
        // Do not allow to use several shader{} and shader_params{} nodes.
        bool shaderSet = false;
        bool shaderParamsSet = false;
        for (const auto &n : node.children)
        {
            if (n->type == SHADER_TYPE && !shaderSet)
            {
                auto gpuProgram = parseShaderNode(*n);
                pass->setGpuProgram(gpuProgram);
                shaderSet = true;
            }
            else if (n->type == SHADER_PARAMS_TYPE && !shaderParamsSet)
            {
                parseShaderParamsNode(*n, pass);
                shaderParamsSet = true;
            }
            else if (n->type == SAMPLER_TYPE)
            {
                auto texture = parseSamplerNode(*n);
                pass->setSampler(n->name, texture);
            }
        }

        // FIXME: implicit assumption. Setting up default lighting when no shader provided.
        if (pass->isLit() && !pass->getGpuProgram())
            pass->setGpuProgram(svc().resourceManager().getFactory().createSimpleLightingGpuProgram());

        return pass;
    }

    shared_ptr<GpuProgram> parseShaderNode(const MaterialLibNode &node)
    {
        // First, check for embed program usage.
        auto embedProgramFound = node.keyValues.find("embed");
        if (embedProgramFound != node.keyValues.end())
        {
            std::string embedProgramName = embedProgramFound->second;
            if (embedProgramName == "colored")
                return svc().resourceManager().getFactory().createColoredGpuProgram();
            else if (embedProgramName == "quad_render")
                return svc().resourceManager().getFactory().createRttQuadProgram();
            else
            {
                glog << "Invalid embed program name" << embedProgramName << logwarn;
                return nullptr;
            }
        }

        // Otherwise, create program from passed paths.
        auto vshader = node.keyValues.find("vertex");
        auto fshader = node.keyValues.find("fragment");

        if (vshader == node.keyValues.end() || fshader == node.keyValues.end())
        {
            glog << "Invalid shader properties found while parsing" << m_libPath << logwarn;
            return nullptr;
        }

        return svc().resourceManager().getFactory().createGpuProgram(vshader->second, fshader->second);
    }

    void parseShaderParamsNode(MaterialLibNode &node, shared_ptr<RenderPass> pass)
    {
        for (auto it : node.keyValues)
        {
            RenderPassParam param;
            param.name = it.first;
            param.value = utils::from_string<float>(it.second);

            pass->addPassParam(param);
        }
    }

    shared_ptr<Texture> parseSamplerNode(const MaterialLibNode &node)
    {
        if (!utils::contains_key(node.keyValues, "type"))
        {
            glog << "Can not parse sampler. Missing texture type." << logwarn;
            return nullptr;
        }

        auto type = node.keyValues.find("type")->second;
        auto creationParams = getTextureCreationParams(node.keyValues, m_libPath);

        if (type == "TEXTURE_2D")
        {
            std::string path = node.keyValues.find("path")->second;

            return svc().resourceManager().getFactory().createTexture(path, creationParams, ResourceLoadingMode::ASYNC);
        }
        else if (type == "TEXTURE_CUBE")
        {
            std::string path = node.keyValues.find("path")->second;

            return svc().resourceManager().getFactory().createCubeTexture(path, creationParams, ResourceLoadingMode::ASYNC);
        }

        glog << "Unknown texture type" << type << logwarn;
        return nullptr;

        // TODO: other types are:
        // TEXTURE_1D TEXTURE_2D TEXTURE_3D 
    }

public:
    MaterialLibParser(const std::string &libPath)
        : m_libPath(libPath)
    {

    }

    bool parse(std::istringstream &input, std::vector<shared_ptr<Material>> &output)
    {
        auto root = MaterialLibNode::createTree(input);
        if (!root)
        {
            glog << "Can not parse material library" << m_libPath << logwarn;
            return false;
        }

        for (const auto &n : root->children)
        {
            if (n->type == MATERIAL_TYPE)
            {
                auto material = parseMaterialNode(*n);
                if (material)
                    output.push_back(material);
            }
        }

        // Clean up.
        MaterialLibNode::clean(root);

        return true;
    }
};

MaterialLibFSLoader::MaterialLibFSLoader(const std::string &path)
    : FSResourceLoader(ResourceLoadingMode::IMMEDIATE),
    m_path(path)
{

}

MaterialLib* MaterialLibFSLoader::createDummy()
{
    return new MaterialLib();
}

bool MaterialLibFSLoader::decode(shared_ptr<FileDataSource> source)
{
    std::string filedata(source->getSize(), 0);
    source->getRaw(&filedata[0], source->getSize());

    std::istringstream input(std::move(filedata));

    return MaterialLibParser(m_path).parse(input, m_materials);
}

void MaterialLibFSLoader::onDecoded(Resource *resource)
{
    auto mtlLib = static_cast<MaterialLib*>(resource);

    for (const auto &material : m_materials)
        mtlLib->appendMaterial(material);
}

}
