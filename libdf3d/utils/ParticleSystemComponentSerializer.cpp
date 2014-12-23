#include "df3d_pch.h"
#include "ParticleSystemComponentSerializer.h"

#include <components/ParticleSystemComponent.h>
#include <utils/JsonHelpers.h>
#include <particlesys/SparkInterface.h>
#include <base/Controller.h>
#include <resources/ResourceManager.h>
#include <render/Texture.h>
#include <render/RenderOperation.h>
#include <render/Image.h>

namespace df3d { namespace utils { namespace serializers {

enum class ModelParamType
{
    ENABLED,
    MUTABLE,
    RANDOM,

    INVALID
};

struct SparkModelParam
{
    std::string name;
    ModelParamType type = ModelParamType::INVALID;
    float value = 0.0f;
    float birthValue = 0.0f;
    float deathValue = 0.0f;
    float minValue = 0.0f;
    float maxValue = 0.0f;

    SPK::ModelParam sparkType;
    SPK::ModelParamFlag sparkFlag;

    SparkModelParam(const Json::Value &v, SPK::ModelParam spkType, SPK::ModelParamFlag spkFlag)
        : sparkType(spkType), sparkFlag(spkFlag)
    {
        if (v.empty() || !v.isObject())
            return;

        auto typeStr = v["type"].asString();
        if (typeStr == "enabled")
        {
            type = ModelParamType::ENABLED;
            value = jsonGetValueWithDefault(v["value"], 0.0f);
        }
        else if (typeStr == "mutable")
        {
            type = ModelParamType::MUTABLE;
            birthValue = jsonGetValueWithDefault(v["birth"], 0.0f);
            deathValue = jsonGetValueWithDefault(v["death"], 0.0f);
        }
        else if (typeStr == "random")
        {
            type = ModelParamType::RANDOM;
            minValue = jsonGetValueWithDefault(v["min"], 0.0f);
            maxValue = jsonGetValueWithDefault(v["max"], 0.0f);
        }
        else
            base::glog << "Invalid model parameter type" << static_cast<size_t>(type) << base::logwarn;
    }
};

SPK::Model *parseSparkModel(const Json::Value &modelJson)
{
    if (modelJson.empty())
        return nullptr;

    float minLifeTime = jsonGetValueWithDefault(modelJson["minLifeTime"], 1.0f);
    float maxLifeTime = jsonGetValueWithDefault(modelJson["maxLifeTime"], 1.0f);
    bool immortal = jsonGetValueWithDefault(modelJson["immortal"], false);

    std::vector<SparkModelParam> params;
    params.push_back(SparkModelParam(modelJson["red"], SPK::PARAM_RED, SPK::FLAG_RED));
    params.push_back(SparkModelParam(modelJson["green"], SPK::PARAM_GREEN, SPK::FLAG_GREEN));
    params.push_back(SparkModelParam(modelJson["blue"], SPK::PARAM_BLUE, SPK::FLAG_BLUE));
    params.push_back(SparkModelParam(modelJson["alpha"], SPK::PARAM_ALPHA, SPK::FLAG_ALPHA));
    params.push_back(SparkModelParam(modelJson["size"], SPK::PARAM_SIZE, SPK::FLAG_SIZE));
    params.push_back(SparkModelParam(modelJson["mass"], SPK::PARAM_MASS, SPK::FLAG_MASS));

    int enabledFlags = SPK::FLAG_RED | SPK::FLAG_GREEN | SPK::FLAG_BLUE;
    int mutableFlags = 0;
    int randomFlags = 0;
    for (auto &p : params)
    {
        if (p.type == ModelParamType::INVALID)
            continue;

        enabledFlags |= p.sparkFlag;
        if (p.type == ModelParamType::MUTABLE)
            mutableFlags |= p.sparkFlag;
        else if (p.type == ModelParamType::RANDOM)
            randomFlags |= p.sparkFlag;
    }

    SPK::Model *model = SPK::Model::create(enabledFlags, mutableFlags, randomFlags);
    model->setLifeTime(minLifeTime, maxLifeTime);
    model->setImmortal(immortal);

    for (auto &p : params)
    {
        if (p.type == ModelParamType::INVALID)
            continue;

        if (p.type == ModelParamType::ENABLED)
        {
            model->setParam(p.sparkType, p.value);
        }
        else if (p.type == ModelParamType::MUTABLE)
        {
            model->setParam(p.sparkType, p.birthValue, p.deathValue);
        }
        else if (p.type == ModelParamType::RANDOM)
        {
            model->setParam(p.sparkType, p.minValue, p.maxValue);
        }
    }

    return model;
}

SPK::Emitter *parseSparkEmitter(const Json::Value &emitterJson)
{
    const auto &zoneJson = emitterJson["Zone"];
    if (zoneJson.empty())
    {
        base::glog << "Emitter has no zone" << base::logwarn;
        return nullptr;
    }

    std::string zoneType = zoneJson["type"].asString();
    SPK::Zone *zone = nullptr;

    // Determine zone type.
    if (zoneType == "Point")
    {
        auto position = jsonGetValueWithDefault(zoneJson["position"], glm::vec3());
        zone = SPK::Point::create(particlesys::glmToSpk(position));
    }
    else if (zoneType == "AABox")
    {
        auto position = jsonGetValueWithDefault(zoneJson["position"], glm::vec3());
        auto dimension = jsonGetValueWithDefault(zoneJson["dimension"], glm::vec3());
        zone = SPK::AABox::create(particlesys::glmToSpk(position), particlesys::glmToSpk(dimension));
    }
    else if (zoneType == "Cylinder")
    {
        auto position = jsonGetValueWithDefault(zoneJson["position"], glm::vec3());
        auto direction = jsonGetValueWithDefault(zoneJson["direction"], glm::vec3(0.0f, 1.0f, 0.0f));
        auto radius = jsonGetValueWithDefault(zoneJson["radius"], 1.0f);
        auto length = jsonGetValueWithDefault(zoneJson["length"], 1.0f);
        zone = SPK::Cylinder::create(particlesys::glmToSpk(position), particlesys::glmToSpk(direction), radius, length);
    }
    else if (zoneType == "Line")
    {
        auto p0 = jsonGetValueWithDefault(zoneJson["p0"], glm::vec3());
        auto p1 = jsonGetValueWithDefault(zoneJson["p1"], glm::vec3());
        zone = SPK::Line::create(particlesys::glmToSpk(p0), particlesys::glmToSpk(p1));
    }
    else if (zoneType == "Plane")
    {
        auto position = jsonGetValueWithDefault(zoneJson["position"], glm::vec3());
        auto normal = jsonGetValueWithDefault(zoneJson["normal"], glm::vec3(0.0f, 1.0f, 0.0f));
        zone = SPK::Plane::create(particlesys::glmToSpk(position), particlesys::glmToSpk(normal));
    }
    else if (zoneType == "Ring")
    {
        auto position = jsonGetValueWithDefault(zoneJson["position"], glm::vec3());
        auto normal = jsonGetValueWithDefault(zoneJson["normal"], glm::vec3(0.0f, 1.0f, 0.0f));
        auto minRadius = jsonGetValueWithDefault(zoneJson["minRadius"], 0.0f);
        auto maxRadius = jsonGetValueWithDefault(zoneJson["maxRadius"], 1.0f);
        
        zone = SPK::Ring::create(particlesys::glmToSpk(position), particlesys::glmToSpk(normal), minRadius, maxRadius);
    }
    else if (zoneType == "Sphere")
    {
        auto position = jsonGetValueWithDefault(zoneJson["position"], glm::vec3());
        auto radius = jsonGetValueWithDefault(zoneJson["radius"], 0.0f);
        zone = SPK::Sphere::create(particlesys::glmToSpk(position), radius);
    }
    else
    {
        base::glog << "Unknown zone type" << zoneType << base::logwarn;
        return nullptr;
    }

    std::string emitterType = emitterJson["type"].asString();
    SPK::Emitter *emitter = nullptr;

    // Determine emitter type.
    if (emitterType == "Random")
    {
        emitter = SPK::RandomEmitter::create();
    }
    else if (emitterType == "Straight")
    {
        auto dir = jsonGetValueWithDefault(emitterJson["direction"], glm::vec3(0.0f, 0.0f, -1.0f));
        emitter = SPK::StraightEmitter::create(SPK::Vector3D(dir.x, dir.y, dir.z));
    }
    else if (emitterType == "Static")
    {
        emitter = SPK::StaticEmitter::create();
    }
    else if (emitterType == "Spheric")
    {
        auto dir = jsonGetValueWithDefault(emitterJson["direction"], glm::vec3(0.0f, 0.0f, -1.0f));
        auto angleA = jsonGetValueWithDefault(emitterJson["angleA"], 0.0f);
        auto angleB = jsonGetValueWithDefault(emitterJson["angleB"], 0.0f);

        emitter = SPK::SphericEmitter::create(particlesys::glmToSpk(dir), glm::radians(angleA), glm::radians(angleB));
    }
    else if (emitterType == "Normal")
    {
        emitter = SPK::NormalEmitter::create();
    }
    else
    {
        base::glog << "Unknown emitter type" << emitterType << base::logwarn;
    }

    // Check for valid emitter.
    if (!emitter)
    {
        SPK_Destroy(zone);
        return nullptr;
    }

    // Init emitter.
    float flow = jsonGetValueWithDefault(emitterJson["flow"], 0.0f);
    float minForce = jsonGetValueWithDefault(emitterJson["minForce"], 0.0f);
    float maxForce = jsonGetValueWithDefault(emitterJson["maxForce"], 0.0f);
    int tank = jsonGetValueWithDefault(emitterJson["tank"], -1);
    bool fullGen = !jsonGetValueWithDefault(emitterJson["generateOnZoneBorders"], false);

    emitter->setFlow(flow);
    emitter->setForce(minForce, maxForce);
    emitter->setZone(zone, fullGen);
    emitter->setTank(tank);

    return emitter;
}

void load(components::ParticleSystemComponent *component, const char *vfxDefinitionFile)
{
    auto root = jsonLoadFromFile(vfxDefinitionFile);
    if (root.isNull())
        return;

    const auto &groupsJson = root["groups"];
    if (groupsJson.empty())
    {
        base::glog << "Failed to parse particle system configs. Groups node wasn't found" << base::logwarn;
        return;
    }

    std::vector<SPK::Group *> systemGroups;

    // Parse all particle system groups.
    for (const auto &groupJson : groupsJson)
    {
        std::string groupName = groupJson["name"].asString();

        // Parse group model.
        const auto &modelJson = groupJson["Model"];
        SPK::Model *model = parseSparkModel(groupJson["Model"]);
        if (!model)
        {
            base::glog << "Model for particle system group" << groupName << "wasn't found" << base::logwarn;
            continue;
        }

        // Parse group emitters.
        std::vector<SPK::Emitter *> emitters;
        const auto &emittersJson = groupJson["Emitters"];
        for (const auto &emitterJson : emittersJson)
        {
            auto emitter = parseSparkEmitter(emitterJson);
            if (emitter)
                emitters.push_back(emitter);
            else
                base::glog << "Failed to parse particle system group emitter" << base::logwarn;
        }

        if (emitters.empty())
        {
            base::glog << "Particle system group has no emitters" << base::logwarn;
            SPK_Destroy(model);
            continue;
        }

        // Get group additional params.
        auto gravity = jsonGetValueWithDefault(groupJson["gravity"], glm::vec3());
        auto friction = jsonGetValueWithDefault(groupJson["friction"], 0.0f);
        bool enableSorting = jsonGetValueWithDefault(groupJson["enableSorting"], false);
        auto maxParticles = jsonGetValueWithDefault(groupJson["maxParticles"], 1000);

        // Get renderer params.
        float scaleX = jsonGetValueWithDefault(groupJson["quadScaleX"], 1.0f);
        float scaleY = jsonGetValueWithDefault(groupJson["quadScaleY"], 1.0f);
        bool depthTest = jsonGetValueWithDefault(groupJson["depthTest"], true);
        bool depthWrite = jsonGetValueWithDefault(groupJson["depthWrite"], true);
        std::string blending = jsonGetValueWithDefault(groupJson["blending"], std::string("none"));

        auto spkBlending = SPK::BLENDING_NONE;
        if (blending == "none")
            spkBlending = SPK::BLENDING_NONE;
        else if (blending == "add")
            spkBlending = SPK::BLENDING_ADD;
        else if (blending == "alpha")
            spkBlending = SPK::BLENDING_ALPHA;
        else
            base::glog << "Unknown blending mode" << blending << base::logwarn;

        // Create particle system renderer.
        particlesys::QuadParticleSystemRenderer *renderer = particlesys::QuadParticleSystemRenderer::create(scaleX, scaleY);
        renderer->enableRenderingHint(SPK::DEPTH_TEST, depthTest);
        renderer->enableRenderingHint(SPK::DEPTH_WRITE, depthWrite);
        renderer->setBlending(spkBlending);

        std::string pathToTexture = jsonGetValueWithDefault(groupJson["texture"], std::string());
        SPK::TexturingMode textureMode = SPK::TEXTURE_NONE;
        if (!pathToTexture.empty())
        {
            auto textureImage = g_resourceManager->getResource<render::Image>(pathToTexture.c_str());
            if (textureImage && textureImage->valid())
            {
                auto texture = make_shared<render::Texture>();
                texture->setImage(textureImage);
                renderer->setDiffuseMap(texture);
                textureMode = SPK::TEXTURE_2D;
            }
        }

        renderer->setTexturingMode(textureMode);

        // Create a group.
        SPK::Group *particlesGroup = SPK::Group::create(model, maxParticles);
        particlesGroup->setGravity(SPK::Vector3D(gravity.x, gravity.y, gravity.z));
        particlesGroup->setFriction(friction);
        particlesGroup->enableSorting(enableSorting);
        particlesGroup->setRenderer(renderer);

        // Add emitters.
        for (auto emitter : emitters)
            particlesGroup->addEmitter(emitter);

        // Store group.
        systemGroups.push_back(particlesGroup);
    }

    if (systemGroups.empty())
    {
        base::glog << "Particle system has no groups" << base::logwarn;
        return;
    }

    component->m_systemLifeTime = jsonGetValueWithDefault(root["systemLifeTime"], -1.0f);

    // Finally, create a particle system.
    component->m_system = SPK::System::create();

    for (auto group : systemGroups)
    {
        component->m_renderOps.push_back(new render::RenderOperation());
        component->m_system->addGroup(group);
    }
}

void save(const components::ParticleSystemComponent *component, const char *vfxDefinitionFile)
{

}

} } }