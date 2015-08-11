#include "df3d_pch.h"
#include "DecoderMTL.h"

#include <boost/algorithm/string.hpp>

#include <base/SystemsMacro.h>
#include <resources/FileDataSource.h>
#include <render/MaterialLib.h>
#include <render/Material.h>
#include <render/Technique.h>
#include <render/RenderPass.h>
#include <render/GpuProgram.h>
#include <render/Texture2D.h>
#include <render/TextureCube.h>
#include <utils/Utils.h>

namespace df3d { namespace resources {

std::map<std::string, render::RenderPass::WindingOrder> woValues = 
{
    { "CW", render::RenderPass::WindingOrder::CW },
    { "CCW", render::RenderPass::WindingOrder::CCW }
};

std::map<std::string, render::RenderPass::PolygonMode> polygonModeValues =
{
    { "FILL", render::RenderPass::PolygonMode::FILL },
    { "WIRE", render::RenderPass::PolygonMode::WIRE }
};

std::map<std::string, render::RenderPass::FaceCullMode> faceCullModeValues =
{
    { "NONE", render::RenderPass::FaceCullMode::NONE },
    { "BACK", render::RenderPass::FaceCullMode::BACK },
    { "FRONT", render::RenderPass::FaceCullMode::FRONT },
    { "FRONT_AND_BACK", render::RenderPass::FaceCullMode::FRONT_AND_BACK }
};

std::map<std::string, render::TextureFiltering> textureFilteringValues =
{
    { "NEAREST", render::TextureFiltering::NEAREST },
    { "BILINEAR", render::TextureFiltering::BILINEAR },
    { "TRILINEAR", render::TextureFiltering::TRILINEAR }
};

std::map<std::string, render::Texture::WrapMode> textureWrapValues =
{
    { "WRAP", render::Texture::WrapMode::WRAP },
    { "CLAMP", render::Texture::WrapMode::CLAMP }
};

std::map<std::string, render::RenderPass::BlendingMode> blendModeValues = 
{
    { "NONE", render::RenderPass::BlendingMode::NONE },
    { "ALPHA", render::RenderPass::BlendingMode::ALPHA },
    { "ADDALPHA", render::RenderPass::BlendingMode::ADDALPHA }
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

        base::glog << "Invalid pass parameter" << param << "found while parsing" << libName << ". Accepted values are :" << acceptedString << base::logwarn;
    }
}

shared_ptr<render::Texture> createTextureOfType(const std::string &type, const std::map<std::string, std::string> &keyValues)
{
    if (type == "TEXTURE_2D")
    {
        std::string path = keyValues.find("path")->second;

        return g_resourceManager->createTexture(path, ResourceLoadingMode::ASYNC);
    }
    else if (type == "TEXTURE_CUBE")
    {
        std::string negativex = keyValues.find("negative_x")->second;
        std::string positivex = keyValues.find("positive_x")->second;
        std::string negativey = keyValues.find("negative_y")->second;
        std::string positivey = keyValues.find("positive_y")->second;
        std::string negativez = keyValues.find("negative_z")->second;
        std::string positivez = keyValues.find("positive_z")->second;

        return g_resourceManager->createCubeTexture(positivex, negativex, positivey, negativey, positivez, negativez, ResourceLoadingMode::ASYNC);
    }

    base::glog << "Unknown texture type" << type << base::logwarn;
    return nullptr;

    // TODO: other types are:
    // TEXTURE_1D TEXTURE_2D TEXTURE_3D 
}

void setTextureBaseParams(shared_ptr<render::Texture> texture, const std::map<std::string, std::string> keyValues, const std::string &libName)
{
    if (!texture)
        return;
    for (const auto &keyval : keyValues)
    {
        if (keyval.first == "filtering")
        {
            std::function<void(render::TextureFiltering)> fn = std::bind(&render::Texture::setFilteringMode, texture.get(), std::placeholders::_1);
            setPassParam(keyval.first, keyval.second, textureFilteringValues, fn, libName);
        }
        else if (keyval.first == "wrap_mode")
        {
            std::function<void(render::Texture::WrapMode)> fn = std::bind(&render::Texture::setWrapMode, texture.get(), std::placeholders::_1);
            setPassParam(keyval.first, keyval.second, textureWrapValues, fn, libName);
        }
        else if (keyval.first == "mipmaps")
        {
            std::function<void(bool)> fn = std::bind(&render::Texture::setMipmapped, texture.get(), std::placeholders::_1);
            setPassParam(keyval.first, keyval.second, boolValues, fn, libName);
        }
        else if (keyval.first == "anisotropy")
        {
            if (keyval.second == "max")
            {
                texture->setMaxAnisotropy(render::ANISOTROPY_LEVEL_MAX);
            }
            else
            {
                auto anisotropyLevel = utils::from_string<int>(keyval.second);
                texture->setMaxAnisotropy(anisotropyLevel);
            }
        }
    }
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
            boost::trim_left(tok);
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

                skippingLines = !utils::contains(render::MaterialLib::Defines, define);
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

            boost::trim(value);

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

shared_ptr<render::Material> DecoderMTL::parseMaterialNode(MaterialLibNode &node)
{
    if (node.name.empty())
    {
        base::glog << "Invalid material name found while parsing" << m_libName << base::logwarn;
        return nullptr;
    }

    auto material = shared_ptr<render::Material>(new render::Material(node.name));

    std::string defaultTechniqueName;
    for (const auto &n : node.children)
    {
        if (n->type == TECHNIQUE_TYPE)
        {
            auto technique = parseTechniqueNode(*n);

            // Set first technique as default.
            if (defaultTechniqueName.empty())
                defaultTechniqueName = technique->getName();

            material->appendTechnique(technique);
        }
    }

    material->setCurrentTechnique(defaultTechniqueName);

    return material;
}

shared_ptr<render::Technique> DecoderMTL::parseTechniqueNode(MaterialLibNode &node)
{
    if (node.name.empty())
    {
        base::glog << "Invalid technique name found while parsing" << m_libName << base::logwarn;
        return nullptr;
    }

    auto technique = shared_ptr<render::Technique>(new render::Technique(node.name));

    for (const auto &n : node.children)
    {
        if (n->type == PASS_TYPE)
        {
            auto pass = parsePassNode(*n);
            technique->appendPass(pass);
        }
    }

    return technique;
}

shared_ptr<render::RenderPass> DecoderMTL::parsePassNode(MaterialLibNode &node)
{
    auto pass = shared_ptr<render::RenderPass>(new render::RenderPass(node.name));

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
            std::function<void(render::RenderPass::FaceCullMode)> fn = std::bind(&render::RenderPass::setFaceCullMode, pass.get(), std::placeholders::_1);
            setPassParam(keyval.first, keyval.second, faceCullModeValues, fn, m_libName);
        }
        else if (keyval.first == "front_face")
        {
            std::function<void(render::RenderPass::WindingOrder)> fn = std::bind(&render::RenderPass::setFrontFaceWinding, pass.get(), std::placeholders::_1);
            setPassParam(keyval.first, keyval.second, woValues, fn, m_libName);
        }
        else if (keyval.first == "polygon_mode")
        {
            std::function<void(render::RenderPass::PolygonMode)> fn = std::bind(&render::RenderPass::setPolygonDrawMode, pass.get(), std::placeholders::_1);
            setPassParam(keyval.first, keyval.second, polygonModeValues, fn, m_libName);
        }
        else if (keyval.first == "depth_test")
        {
            std::function<void(bool)> fn = std::bind(&render::RenderPass::enableDepthTest, pass.get(), std::placeholders::_1);
            setPassParam(keyval.first, keyval.second, boolValues, fn, m_libName);
        }
        else if (keyval.first == "depth_write")
        {
            std::function<void(bool)> fn = std::bind(&render::RenderPass::enableDepthWrite, pass.get(), std::placeholders::_1);
            setPassParam(keyval.first, keyval.second, boolValues, fn, m_libName);
        }
        else if (keyval.first == "is_lit")
        {
            std::function<void(bool)> fn = std::bind(&render::RenderPass::enableLighting, pass.get(), std::placeholders::_1);
            setPassParam(keyval.first, keyval.second, boolValues, fn, m_libName);
        }
        else if (keyval.first == "blend")
        {
            std::function<void(render::RenderPass::BlendingMode)> fn = std::bind(&render::RenderPass::setBlendMode, pass.get(), std::placeholders::_1);
            setPassParam(keyval.first, keyval.second, blendModeValues, fn, m_libName);
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

    return pass;
}

shared_ptr<render::GpuProgram> DecoderMTL::parseShaderNode(MaterialLibNode &node)
{
    // First, check for embed program usage.
    std::string embedProgram = node.keyValues["embed"];
    if (!embedProgram.empty())
    {
        std::string embedProgramName;
        if (embedProgram == "colored")
            return g_resourceManager->createColoredGpuProgram();
        else if (embedProgram == "quad_render")
            return g_resourceManager->createRttQuadProgram();
        else
        {
            base::glog << "Invalid embed program name" << embedProgram << base::logwarn;
            return nullptr;
        }
    }

    // Otherwise, create program from passed paths.
    std::string vshader, fshader;
    vshader = node.keyValues["vertex"];
    fshader = node.keyValues["fragment"];

    if (vshader.empty() || fshader.empty())
    {
        base::glog << "Invalid shader properties found while parsing" << m_libName << base::logwarn;
        return nullptr;
    }

    return g_resourceManager->createGpuProgram(vshader, fshader);
}

void DecoderMTL::parseShaderParamsNode(MaterialLibNode &node, shared_ptr<render::RenderPass> pass)
{
    for (auto it : node.keyValues)
    {
        render::RenderPassParam param;
        param.name = it.first;
        param.value = utils::from_string<float>(it.second);

        pass->addPassParam(param);
    }
}

shared_ptr<render::Texture> DecoderMTL::parseSamplerNode(MaterialLibNode &node)
{
    if (!utils::contains_key(node.keyValues, "type"))
    {
        base::glog << "Can not parse sampler. Missing texture type." << base::logwarn;
        return nullptr;
    }

    auto texture = createTextureOfType(node.keyValues["type"], node.keyValues);
    setTextureBaseParams(texture, node.keyValues, m_libName);

    return texture;
}

DecoderMTL::DecoderMTL()
{

}

DecoderMTL::~DecoderMTL()
{

}

shared_ptr<Resource> DecoderMTL::createResource()
{
    return make_shared<render::MaterialLib>();
}

bool DecoderMTL::decodeResource(shared_ptr<FileDataSource> file, shared_ptr<Resource> resource)
{
    if (!file || !file->valid())
        return false;

    auto mtllib = dynamic_pointer_cast<render::MaterialLib>(resource);
    if (!mtllib)
        return false;

    m_libName = mtllib->getGUID();

    std::string filedata(file->getSize(), 0);
    file->getRaw(&filedata[0], file->getSize());

    std::istringstream input(std::move(filedata));
    
    auto root = MaterialLibNode::createTree(input);
    if (!root)
    {
        base::glog << "Can not parse material library" << m_libName << base::logwarn;
        return false;
    }

    for (const auto &n : root->children)
    {
        if (n->type == MATERIAL_TYPE)
        {
            auto material = parseMaterialNode(*n);
            mtllib->appendMaterial(material);
        }
    }
    
    // Clean up.
    MaterialLibNode::clean(root);

    return mtllib->materialCount() != 0;
}

} }
