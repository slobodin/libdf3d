#include "df3d_pch.h"
#include "DecoderMTL.h"

#include <base/Controller.h>
#include <resources/FileSystem.h>
#include <resources/FileDataSource.h>
#include <resources/ResourceManager.h>
#include <render/MaterialLib.h>
#include <render/Material.h>
#include <render/Technique.h>
#include <render/RenderPass.h>
#include <render/GpuProgram.h>
#include <render/Texture.h>
#include <render/Image.h>
#include <utils/Utils.h>

// Boost.spirit didn't satisfy me. Reason: code bloat, compile time.
//#define FUSION_MAX_VECTOR_SIZE 20
//#define SPIRIT_ARGUMENTS_LIMIT 20
//
//#include <boost/fusion/adapted.hpp>
//#include <boost/spirit/home/phoenix.hpp>
//#include <boost/spirit/home/qi.hpp>

namespace df3d { namespace resources {

std::map<std::string, render::RenderPass::WindingOrder> woValues = 
{
    std::make_pair("CW", render::RenderPass::WO_CW),
    std::make_pair("CCW", render::RenderPass::WO_CCW)
};

std::map<std::string, render::RenderPass::PolygonMode> polygonModeValues =
{
    std::make_pair("FILL", render::RenderPass::PM_FILL),
    std::make_pair("WIRE", render::RenderPass::PM_WIRE)
};

std::map<std::string, render::RenderPass::FaceCullMode> faceCullModeValues =
{
    std::make_pair("NONE", render::RenderPass::FCM_NONE),
    std::make_pair("BACK", render::RenderPass::FCM_BACK),
    std::make_pair("FRONT", render::RenderPass::FCM_FRONT),
    std::make_pair("FRONT_AND_BACK", render::RenderPass::FCM_FRONT_AND_BACK)
};

std::map<std::string, render::Texture::Type> textureTypeValues = 
{
    std::make_pair("TEXTURE_1D", render::Texture::TEXTURE_1D),
    std::make_pair("TEXTURE_2D", render::Texture::TEXTURE_2D),
    std::make_pair("TEXTURE_3D", render::Texture::TEXTURE_3D),
    std::make_pair("TEXTURE_CUBE", render::Texture::TEXTURE_CUBE)
};

std::map<std::string, render::Texture::Filtering> textureFilteringValues =
{
    std::make_pair("NEAREST", render::Texture::NEAREST),
    std::make_pair("BILINEAR", render::Texture::BILINEAR),
    std::make_pair("TRILINEAR", render::Texture::TRILINEAR)
};

std::map<std::string, render::Texture::WrapMode> textureWrapValues =
{
    std::make_pair("WRAP", render::Texture::WM_WRAP),
    std::make_pair("CLAMP", render::Texture::WM_CLAMP)
};

std::map<std::string, render::RenderPass::BlendingMode> blendModeValues = 
{
    std::make_pair("NONE", render::RenderPass::BM_NONE),
    std::make_pair("ALPHA", render::RenderPass::BM_ALPHA),
    std::make_pair("ADDALPHA", render::RenderPass::BM_ADDALPHA)
};

std::map<std::string, bool> boolValues =
{
    std::make_pair("true", true),
    std::make_pair("false", false),
    std::make_pair("1", true),
    std::make_pair("0", false)
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
        return std::find(std::begin(NodesTypes), std::end(NodesTypes), name) != std::end(NodesTypes);
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
        while (is >> tok)
        {
            // Skip empty lines.
            boost::trim_left(tok);
            if (tok.empty())
                continue;

            // Skip comment lines.
            if (boost::starts_with(tok, "//"))
            {
                utils::skipLine(is);
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

    material->setCurrentTechnique(defaultTechniqueName.c_str());

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
            pass->setSampler(n->name.c_str(), texture);
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
            embedProgramName = render::COLORED_PROGRAM_EMBED_PATH;
        else if (embedProgram == "quad_render")
            embedProgramName = render::RTT_QUAD_PROGRAM_EMBED_PATH;
        else
        {
            base::glog << "Invalid embed program name" << embedProgram << base::logwarn;
            return nullptr;
        }

        return g_resourceManager->getResource<render::GpuProgram>(embedProgramName.c_str());
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

    return render::GpuProgram::create(vshader.c_str(), fshader.c_str());
}

void DecoderMTL::parseShaderParamsNode(MaterialLibNode &node, shared_ptr<render::RenderPass> pass)
{
    for (auto it : node.keyValues)
    {
        render::RenderPassParam param;
        param.name = it.first;
        param.value = boost::lexical_cast<float>(it.second);

        pass->addPassParam(param);
    }
}

shared_ptr<render::Texture> DecoderMTL::parseSamplerNode(MaterialLibNode &node)
{
    // WORKAROUND for cubemaps:
    for (auto &keyval : node.keyValues)
    {
        if (keyval.first == "type" && keyval.second == "TEXTURE_CUBE")
            return parseCubemapSamplerNode(node);
    }

    std::string path = node.keyValues["path"];

    auto textureImage = g_resourceManager->getResource<render::Image>(path.c_str(), ResourceManager::LOAD_MODE_ASYNC);
    if (!textureImage)
        return nullptr;

    auto texture = make_shared<render::Texture>();
    texture->setImage(textureImage);

    for (auto &keyval : node.keyValues)
    {
        if (keyval.first == "type")
        {
            std::function<void(render::Texture::Type)> fn = std::bind(&render::Texture::setType, texture.get(), std::placeholders::_1);
            setPassParam(keyval.first, keyval.second, textureTypeValues, fn, m_libName);
        }
        else if (keyval.first == "filtering")
        {
            std::function<void(render::Texture::Filtering)> fn = std::bind(&render::Texture::setFilteringMode, texture.get(), std::placeholders::_1);
            setPassParam(keyval.first, keyval.second, textureFilteringValues, fn, m_libName);
        }
        else if (keyval.first == "wrap_mode")
        {
            std::function<void(render::Texture::WrapMode)> fn = std::bind(&render::Texture::setWrapMode, texture.get(), std::placeholders::_1);
            setPassParam(keyval.first, keyval.second, textureWrapValues, fn, m_libName);
        }
        else if (keyval.first == "mipmaps")
        {
            std::function<void(bool)> fn = std::bind(&render::Texture::setMipmapped, texture.get(), std::placeholders::_1);
            setPassParam(keyval.first, keyval.second, boolValues, fn, m_libName);
        }
    }

    return texture;
}

shared_ptr<render::Texture> DecoderMTL::parseCubemapSamplerNode(MaterialLibNode &node)
{
    //std::string pathPostfix[] = { "left", "right", "up", "down", "front", "back" };

    //std::vector<shared_ptr<resources::Resource>> cubemapParts;

    //std::vector<unsigned char> cubemapData;
    //for (auto &postfix : pathPostfix)
    //{
    //    auto path = node.keyValues["path_" + postfix];
    //    auto texture = g_resourceManager->getResource<render::Texture>(path, ResourceManager::LOAD_MODE_IMMEDIATE);
    //    if (!texture || !texture->valid())
    //    {
    //        base::glog << "Cubemap texture part" << postfix << "not found or invalid" << base::logwarn;
    //        // FIXME:
    //        // Cleanup resource manager.
    //        return nullptr;
    //    }

    //    cubemapData.insert(cubemapData.end(), texture->data(), texture->data() + texture->width() * texture->height() * texture->depth());

    //    cubemapParts.push_back(texture);
    //}

    //auto cubemap = make_shared<render::Texture>();
    //cubemap->setWithData(&cubemapData[0], source_dimensions.x, source_dimensions.y, 4 * source_dimensions.x, render::Texture::PF_RGBA);

    //cubemap->setInitialized();

    //cubemap->setType(render::Texture::TEXTURE_CUBE);
    //cubemap->setFilteringMode(render::Texture::BILINEAR);
    //cubemap->setMipmapped(false);

    //for (auto part : cubemapParts)
    //{
    //    g_resourceManager->unloadResource(part);
    //}

    return nullptr;
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

bool DecoderMTL::decodeResource(const shared_ptr<FileDataSource> file, shared_ptr<Resource> resource)
{
    if (!file || !file->valid())
        return false;

    auto mtllib = boost::dynamic_pointer_cast<render::MaterialLib>(resource);
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
