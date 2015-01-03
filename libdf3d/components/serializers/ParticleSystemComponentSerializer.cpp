#include "df3d_pch.h"
#include "ParticleSystemComponentSerializer.h"

#include <components/ParticleSystemComponent.h>
#include <utils/JsonHelpers.h>
#include <utils/Utils.h>
#include <particlesys/SparkInterface.h>
#include <base/Controller.h>
#include <resources/ResourceManager.h>
#include <render/Texture.h>
#include <render/RenderOperation.h>
#include <render/Image.h>
#include <render/RenderPass.h>

namespace df3d { namespace components { namespace serializers {

std::map<std::string, SPK::Param> StringToSparkParam = 
{
    { "angle", SPK::PARAM_ANGLE },
    { "mass", SPK::PARAM_MASS },
    { "rotation_speed", SPK::PARAM_ROTATION_SPEED },
    { "scale", SPK::PARAM_SCALE },
    { "texture_index", SPK::PARAM_TEXTURE_INDEX }
};

SPK::Ref<SPK::ColorInterpolator> parseSparkColorInterpolator(const Json::Value &interpolatorJson)
{
    if (interpolatorJson.empty())
        return SPK_NULL_REF;

    auto birthColor = utils::jsonGetValueWithDefault(interpolatorJson["birth"], glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
    auto deathColor = utils::jsonGetValueWithDefault(interpolatorJson["death"], glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
    birthColor *= 255.0f;
    deathColor *= 255.0f;

    auto c1 = SPK::Color((int)birthColor.r, (int)birthColor.g, (int)birthColor.b, (int)birthColor.a);
    auto c2 = SPK::Color((int)birthColor.r, (int)birthColor.g, (int)birthColor.b, (int)birthColor.a);
    return SPK::ColorSimpleInterpolator::create(c1, c2);
}

SPK::Ref<SPK::FloatSimpleInterpolator> parseSparkParamInterpolator(const Json::Value &interpolatorJson)
{
    if (interpolatorJson.empty())
        return SPK_NULL_REF;

    auto birthValue = utils::jsonGetValueWithDefault(interpolatorJson["birth"], 0.0f);
    auto deathValue = utils::jsonGetValueWithDefault(interpolatorJson["death"], 0.0f);

    return SPK::FloatSimpleInterpolator::create(birthValue, deathValue);
}

SPK::Ref<SPK::Emitter> parseSparkEmitter(const Json::Value &emitterJson)
{
    using namespace utils;

    const auto &zoneJson = emitterJson["Zone"];
    if (zoneJson.empty())
    {
        base::glog << "Emitter has no zone" << base::logwarn;
        return SPK_NULL_REF;
    }

    std::string zoneType = zoneJson["type"].asString();
    SPK::Ref<SPK::Zone> zone = SPK_NULL_REF;

    // Determine zone type.
    if (zoneType == "Point")
    {
        auto position = jsonGetValueWithDefault(zoneJson["position"], glm::vec3());
        zone = SPK::Point::create(particlesys::glmToSpk(position));
    }
    else if (zoneType == "Box")
    {
        auto position = jsonGetValueWithDefault(zoneJson["position"], glm::vec3());
        auto dimension = jsonGetValueWithDefault(zoneJson["dimension"], glm::vec3(1.0f, 1.0f, 1.0f));
        auto front = jsonGetValueWithDefault(zoneJson["front"], glm::vec3(0.0f, 0.0f, 1.0f));
        auto up = jsonGetValueWithDefault(zoneJson["up"], glm::vec3(0.0f, 1.0f, 0.0f));
        zone = SPK::Box::create(particlesys::glmToSpk(position), particlesys::glmToSpk(dimension), particlesys::glmToSpk(front), particlesys::glmToSpk(up));
    }
    else if (zoneType == "Cylinder")
    {
        auto position = jsonGetValueWithDefault(zoneJson["position"], glm::vec3());
        auto axis = jsonGetValueWithDefault(zoneJson["axis"], glm::vec3(0.0f, 1.0f, 0.0f));
        auto radius = jsonGetValueWithDefault(zoneJson["radius"], 1.0f);
        auto height = jsonGetValueWithDefault(zoneJson["height"], 1.0f);
        zone = SPK::Cylinder::create(particlesys::glmToSpk(position), height, radius, particlesys::glmToSpk(axis));
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
        auto radius = jsonGetValueWithDefault(zoneJson["radius"], 1.0f);
        zone = SPK::Sphere::create(particlesys::glmToSpk(position), radius);
    }
    else
    {
        base::glog << "Unknown zone type" << zoneType << base::logwarn;
        return SPK_NULL_REF;
    }

    std::string emitterType = emitterJson["type"].asString();
    SPK::Ref<SPK::Emitter> emitter = SPK_NULL_REF;

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
        return SPK_NULL_REF;

    // Init emitter.
    float flow = jsonGetValueWithDefault(emitterJson["flow"], 0.0f);
    float minForce = jsonGetValueWithDefault(emitterJson["minForce"], 0.0f);
    float maxForce = jsonGetValueWithDefault(emitterJson["maxForce"], 0.0f);
    int tank = jsonGetValueWithDefault(emitterJson["tank"], -1);
    bool fullGen = !jsonGetValueWithDefault(emitterJson["generateOnZoneBorders"], false);

    emitter->setTank(tank);
    emitter->setFlow(flow);
    emitter->setForce(minForce, maxForce);
    emitter->setZone(zone, fullGen);

    return emitter;
}

shared_ptr<NodeComponent> ParticleSystemComponentSerializer::fromJson(const Json::Value &root)
{
    using namespace utils;

    const auto &groupsJson = root["groups"];
    if (groupsJson.empty())
    {
        base::glog << "Failed to parse particle system configs. Groups node wasn't found" << base::logwarn;
        return nullptr;
    }

    std::vector<SPK::Ref<SPK::Group>> systemGroups;

    // Parse all particle system groups.
    for (const auto &groupJson : groupsJson)
    {
        std::string groupName = groupJson["name"].asString();

        // Parse group emitters.
        std::vector<SPK::Ref<SPK::Emitter>> emitters;
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
            continue;
        }

        // Get group additional params.
        auto gravity = jsonGetValueWithDefault(groupJson["gravity"], glm::vec3());
        auto friction = jsonGetValueWithDefault(groupJson["friction"], 0.0f);
        bool enableSorting = jsonGetValueWithDefault(groupJson["enableSorting"], false);
        auto maxParticles = jsonGetValueWithDefault(groupJson["maxParticles"], 1000);
        float minLifeTime = jsonGetValueWithDefault(groupJson["minLifeTime"], 1.0f);
        float maxLifeTime = jsonGetValueWithDefault(groupJson["maxLifeTime"], 1.0f);
        bool immortal = jsonGetValueWithDefault(groupJson["immortal"], false);
        float radius = jsonGetValueWithDefault(groupJson["radius"], 1.0f);

        // Get renderer params.
        float scaleX = jsonGetValueWithDefault(groupJson["quadScaleX"], 1.0f);
        float scaleY = jsonGetValueWithDefault(groupJson["quadScaleY"], 1.0f);
        bool depthTest = jsonGetValueWithDefault(groupJson["depthTest"], true);
        bool depthWrite = jsonGetValueWithDefault(groupJson["depthWrite"], true);
        std::string blending = jsonGetValueWithDefault(groupJson["blending"], std::string("none"));

        auto spkBlending = SPK::BLEND_MODE_NONE;
        if (blending == "none")
            spkBlending = SPK::BLEND_MODE_NONE;
        else if (blending == "add")
            spkBlending = SPK::BLEND_MODE_ADD;
        else if (blending == "alpha")
            spkBlending = SPK::BLEND_MODE_ALPHA;
        else
            base::glog << "Unknown blending mode" << blending << base::logwarn;

        // Create particle system renderer.
        // TODO:
        // Can be more types of renderers.
        // FIXME: can share renderer.
        auto renderer = particlesys::QuadParticleSystemRenderer::create(scaleX, scaleY);
        renderer->enableRenderingOption(SPK::RENDERING_OPTION_DEPTH_WRITE, depthWrite);
        renderer->m_pass->enableDepthTest(depthTest);
        renderer->setBlendMode(spkBlending);

        std::string pathToTexture = jsonGetValueWithDefault(groupJson["texture"], std::string());
        SPK::TextureMode textureMode = SPK::TEXTURE_MODE_NONE;
        if (!pathToTexture.empty())
        {
            auto textureImage = g_resourceManager->getResource<render::Image>(pathToTexture.c_str());
            if (textureImage && textureImage->valid())
            {
                auto texture = make_shared<render::Texture>();
                texture->setImage(textureImage);
                renderer->setDiffuseMap(texture);
                textureMode = SPK::TEXTURE_MODE_2D;
            }
        }

        renderer->setTexturingMode(textureMode);

        // Create a group.
        auto particlesGroup = SPK::Group::create(maxParticles);
        particlesGroup->addModifier(SPK::Gravity::create({ gravity.x, gravity.y, gravity.z }));
        particlesGroup->addModifier(SPK::Friction::create(friction));
        particlesGroup->enableSorting(enableSorting);
        particlesGroup->setRenderer(renderer);
        particlesGroup->setLifeTime(minLifeTime, maxLifeTime);
        particlesGroup->setImmortal(immortal);
        particlesGroup->setRadius(radius);

        // Add emitters.
        for (auto emitter : emitters)
            particlesGroup->addEmitter(emitter);

        // Add color interpolator if any.
        auto colorInterpolatorJson = parseSparkColorInterpolator(groupJson["ColorInterpolator"]);
        if (colorInterpolatorJson)
            particlesGroup->setColorInterpolator(colorInterpolatorJson);

        // Add param interpolators.
        auto paramInterpolatorsJson = groupJson["ParamInterpolators"];
        for (auto it = paramInterpolatorsJson.begin(); it != paramInterpolatorsJson.end(); it++)
        {
            auto key = it.key().asString();
            if (!utils::contains_key(StringToSparkParam, key))
            {
                base::glog << "Unknown spark param interpolator" << key << base::logwarn;
                continue;
            }

            particlesGroup->setParamInterpolator(StringToSparkParam[key], parseSparkParamInterpolator(*it));
        }

        // Store group.
        systemGroups.push_back(particlesGroup);
    }

    if (systemGroups.empty())
    {
        base::glog << "Particle system has no groups" << base::logwarn;
        return nullptr;
    }

    auto worldTransformed = jsonGetValueWithDefault(root["worldTransformed"], true);

    auto result = make_shared<ParticleSystemComponent>();

    result->setWorldTransformed(worldTransformed);
    result->setSystemLifeTime(jsonGetValueWithDefault(root["systemLifeTime"], -1.0f));

    for (auto group : systemGroups)
        result->addSPKGroup(group);

    result->initializeSPK();

    return result;
}

Json::Value ParticleSystemComponentSerializer::toJson(shared_ptr<const NodeComponent> component)
{
    assert(false);
    // TODO:
    return Json::Value();
}

} } }