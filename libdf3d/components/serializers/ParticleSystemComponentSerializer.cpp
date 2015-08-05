#include "df3d_pch.h"
#include "ParticleSystemComponentSerializer.h"

#include <components/ParticleSystemComponent.h>
#include <utils/JsonHelpers.h>
#include <utils/Utils.h>
#include <particlesys/SparkInterface.h>
#include <render/Texture2D.h>
#include <render/RenderOperation.h>
#include <render/RenderPass.h>
#include <base/SystemsMacro.h>

namespace df3d { namespace components { namespace serializers {

std::map<std::string, SPK::Param> StringToSparkParam =
{
    { "angle", SPK::PARAM_ANGLE },
    { "mass", SPK::PARAM_MASS },
    { "rotation_speed", SPK::PARAM_ROTATION_SPEED },
    { "scale", SPK::PARAM_SCALE },
    { "texture_index", SPK::PARAM_TEXTURE_INDEX }
};

std::map<std::string, SPK::OrientationPreset> StringToSparkDirection =
{
    { "direction_aligned", SPK::DIRECTION_ALIGNED },
    { "camera_plane_aligned", SPK::CAMERA_PLANE_ALIGNED },
    { "camera_point_aligned", SPK::CAMERA_POINT_ALIGNED },
    { "around_axis", SPK::AROUND_AXIS },
    { "towards_point", SPK::TOWARDS_POINT },
    { "fixed_orientation", SPK::FIXED_ORIENTATION },
};

SPK::Color toSpkColor(glm::vec4 color)
{
    color *= 255.0f;
    return SPK::Color((int)color.r, (int)color.g, (int)color.b, (int)color.a);
}

SPK::Ref<SPK::ColorSimpleInterpolator> parseSparkSimpleColorInterpolator(const Json::Value &dataJson)
{
    glm::vec4 birthColor(0.0f, 0.0f, 0.0f, 1.0f);
    glm::vec4 deathColor(0.0f, 0.0f, 0.0f, 1.0f);

    dataJson["birth"] >> birthColor;
    dataJson["death"] >> deathColor;

    return SPK::ColorSimpleInterpolator::create(toSpkColor(birthColor), toSpkColor(deathColor));
}

SPK::Ref<SPK::ColorGraphInterpolator> parseSparkGraphColorInterpolator(const Json::Value &dataJson)
{
    auto result = SPK::ColorGraphInterpolator::create();

    for (const auto &entryJson : dataJson)
    {
        float x = 0.0f;
        glm::vec4 y(0.0f, 0.0f, 0.0f, 1.0f);

        entryJson["x"] >> x;
        entryJson["y"] >> y;

        result->addEntry(x, toSpkColor(y));
    }

    return result;
}

SPK::Ref<SPK::ColorInterpolator> parseSparkColorInterpolator(const Json::Value &interpolatorJson)
{
    if (interpolatorJson.empty())
        return SPK_NULL_REF;

    const auto &dataJson = interpolatorJson["data"];
    if (dataJson.empty())
    {
        base::glog << "Failed to parse spark color interpolator. Empty data field" << base::logwarn;
        return SPK_NULL_REF;
    }

    auto typeStr = interpolatorJson["type"].asString();
    if (typeStr == "simple")
        return parseSparkSimpleColorInterpolator(dataJson);
    else if (typeStr == "graph")
        return parseSparkGraphColorInterpolator(dataJson);

    base::glog << "Unknown spark color interpolator type" << typeStr << base::logwarn;
    return SPK_NULL_REF;
}

SPK::Ref<SPK::FloatSimpleInterpolator> parseSparkSimpleInterpolator(const Json::Value &dataJson)
{
    float birthValue = 0.0f;
    float deathValue = 0.0f;

    dataJson["birth"] >> birthValue;
    dataJson["death"] >> deathValue;

    return SPK::FloatSimpleInterpolator::create(birthValue, deathValue);
}

SPK::Ref<SPK::FloatRandomInterpolator> parseSparkRandomInterpolator(const Json::Value &dataJson)
{
    const auto &birthRange = dataJson["birth"];
    const auto &deathRange = dataJson["death"];

    float minBirth = birthRange[0].asFloat();
    float maxBirth = birthRange[1].asFloat();
    float minDeath = deathRange[0].asFloat();
    float maxDeath = deathRange[1].asFloat();

    return SPK::FloatRandomInterpolator::create(minBirth, maxBirth, minDeath, maxDeath);
}

SPK::Ref<SPK::FloatGraphInterpolator> parseSparkGraphInterpolator(const Json::Value &dataJson)
{
    auto result = SPK::FloatGraphInterpolator::create();

    for (const auto &entryJson : dataJson)
    {
        float x = 0.0f;
        float y = 0.0f;

        entryJson["x"] >> x;
        entryJson["y"] >> y;

        result->addEntry(x, y);
    }

    return result;
}

SPK::Ref<SPK::FloatInterpolator> parseSparkParamInterpolator(const Json::Value &interpolatorJson)
{
    if (interpolatorJson.empty())
        return SPK_NULL_REF;

    const auto &dataJson = interpolatorJson["data"];
    if (dataJson.empty())
    {
        base::glog << "Failed to parse spark float interpolator. Empty data field" << base::logwarn;
        return SPK_NULL_REF;
    }

    auto typeStr = interpolatorJson["type"].asString();
    if (typeStr == "simple")
        return parseSparkSimpleInterpolator(dataJson);
    else if (typeStr == "random")
        return parseSparkRandomInterpolator(dataJson);
    else if (typeStr == "graph")
        return parseSparkGraphInterpolator(dataJson);

    base::glog << "Unknown spark float interpolator type" << typeStr << base::logwarn;
    return SPK_NULL_REF;
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
        glm::vec3 position;
        zoneJson["position"] >> position;
        zone = SPK::Point::create(particlesys::glmToSpk(position));
    }
    else if (zoneType == "Box")
    {
        glm::vec3 position, dimension(1.0f, 1.0f, 1.0f), front(0.0, 0.0, 1.0f), up(0.0f, 1.0f, 0.0f);

        zoneJson["position"] >> position;
        zoneJson["dimension"] >> dimension;
        zoneJson["front"] >> front;
        zoneJson["up"] >> up;

        zone = SPK::Box::create(particlesys::glmToSpk(position), particlesys::glmToSpk(dimension), particlesys::glmToSpk(front), particlesys::glmToSpk(up));
    }
    else if (zoneType == "Cylinder")
    {
        glm::vec3 position, axis(0.0f, 1.0f, 0.0f);
        float radius = 1.0f, height = 1.0f;

        zoneJson["position"] >> position;
        zoneJson["axis"] >> axis;
        zoneJson["radius"] >> radius;
        zoneJson["height"] >> height;

        zone = SPK::Cylinder::create(particlesys::glmToSpk(position), height, radius, particlesys::glmToSpk(axis));
    }
    else if (zoneType == "Plane")
    {
        glm::vec3 position, normal(0.0f, 1.0f, 0.0f);

        zoneJson["position"] >> position;
        zoneJson["normal"] >> normal;

        zone = SPK::Plane::create(particlesys::glmToSpk(position), particlesys::glmToSpk(normal));
    }
    else if (zoneType == "Ring")
    {
        glm::vec3 position, normal(0.0f, 1.0f, 0.0f);
        float minRadius = 0.0f, maxRadius = 1.0f;

        zoneJson["position"] >> position;
        zoneJson["normal"] >> normal;
        zoneJson["minRadius"] >> minRadius;
        zoneJson["maxRadius"] >> maxRadius;

        zone = SPK::Ring::create(particlesys::glmToSpk(position), particlesys::glmToSpk(normal), minRadius, maxRadius);
    }
    else if (zoneType == "Sphere")
    {
        glm::vec3 position;
        float radius = 1.0f;

        zoneJson["position"] >> position;
        zoneJson["radius"] >> radius;

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
        glm::vec3 dir(0.0f, 0.0f, -1.0f);

        emitterJson["direction"] >> dir;

        emitter = SPK::StraightEmitter::create(particlesys::glmToSpk(dir));
    }
    else if (emitterType == "Static")
    {
        emitter = SPK::StaticEmitter::create();
    }
    else if (emitterType == "Spheric")
    {
        glm::vec3 dir(0.0f, 0.0f, -1.0f);
        float angleA = 0.0f, angleB = 0.0f;

        emitterJson["direction"] >> dir;
        emitterJson["angleA"] >> angleA;
        emitterJson["angleB"] >> angleB;

        emitter = SPK::SphericEmitter::create(particlesys::glmToSpk(dir), glm::radians(angleA), glm::radians(angleB));
    }
    else if (emitterType == "Normal")
    {
        emitter = SPK::NormalEmitter::create();
        bool inverted = false;

        emitterJson["inverted"] >> inverted;

        ((SPK::NormalEmitter*)emitter.get())->setInverted(inverted);
    }
    else
    {
        base::glog << "Unknown emitter type" << emitterType << base::logwarn;
    }

    // Check for valid emitter.
    if (!emitter)
        return SPK_NULL_REF;

    // Init emitter.
    float flow = 0.0f, minForce = 0.0f, maxForce = 0.0f;
    int tank = -1;
    bool fullGen = false;

    emitterJson["flow"] >> flow;
    emitterJson["minForce"] >> minForce;
    emitterJson["maxForce"] >> maxForce;
    emitterJson["tank"] >> tank;
    emitterJson["generateOnZoneBorders"] >> fullGen;

    emitter->setTank(tank);
    emitter->setFlow(flow);
    emitter->setForce(minForce, maxForce);
    emitter->setZone(zone, !fullGen);

    return emitter;
}

void parseSparkModifiers(SPK::Ref<SPK::Group> group, const Json::Value &modifiersJson)
{
    for (const auto &modifierJson : modifiersJson)
    {
        auto type = modifierJson["type"].asString();

        if (type == "gravity")
        {
            glm::vec3 val;
            modifierJson["value"] >> val;
            group->addModifier(SPK::Gravity::create({ val.x, val.y, val.z }));
        }
        else if (type == "friction")
        {
            float val = 0.0f;
            modifierJson["value"] >> val;
            group->addModifier(SPK::Friction::create(val));
        }
        else
        {
            df3d::base::glog << "Unknown particle system modifier" << type << df3d::base::logwarn;
        }
    }
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

        // Get group params.
        std::string groupName;
        bool enableSorting = false, immortal = false;
        int maxParticles = 100;
        float minLifeTime = 1.0f, maxLifeTime = 1.0f, radius = 1.0f;

        groupJson["name"] >> groupName;
        groupJson["enableSorting"] >> enableSorting;
        groupJson["maxParticles"] >> maxParticles;
        groupJson["minLifeTime"] >> minLifeTime;
        groupJson["maxLifeTime"] >> maxLifeTime;
        groupJson["immortal"] >> immortal;
        groupJson["radius"] >> radius;

        // Get renderer params.
        float scaleX = 1.0f, scaleY = 1.0f;
        bool depthTest = true, depthWrite = true;
        std::string blending = "none";

        groupJson["quadScaleX"] >> scaleX;
        groupJson["quadScaleY"] >> scaleY;
        groupJson["depthTest"] >> depthTest;
        groupJson["depthWrite"] >> depthWrite;
        groupJson["blending"] >> blending;

        auto spkBlending = SPK::BLEND_MODE_NONE;
        if (blending == "none")
            spkBlending = SPK::BLEND_MODE_NONE;
        else if (blending == "add")
            spkBlending = SPK::BLEND_MODE_ADD;
        else if (blending == "alpha")
            spkBlending = SPK::BLEND_MODE_ALPHA;
        else
            base::glog << "Unknown blending mode" << blending << base::logwarn;

        std::string rendererType("quad");
        groupJson["renderer"] >> rendererType;

        // Create particle system renderer.
        SPK::Ref<particlesys::ParticleSystemRenderer> renderer;
        if (rendererType == "quad")
        {
            auto quadRenderer = particlesys::QuadParticleSystemRenderer::create(scaleX, scaleY);

            // Setup special renderer params.
            if (!groupJson["orientation"].empty())
            {
                auto orientationStr = groupJson["orientation"].asString();
                auto found = StringToSparkDirection.find(orientationStr);
                if (found != StringToSparkDirection.end())
                    quadRenderer->setOrientation(found->second);
                else
                    base::glog << "Unknown spark particle system orientation" << base::logwarn;
            }

            std::string pathToTexture = jsonGetValueWithDefault(groupJson["texture"], std::string());
            SPK::TextureMode textureMode = SPK::TEXTURE_MODE_NONE;
            if (!pathToTexture.empty())
            {
                auto texture = g_resourceManager->createTexture(pathToTexture, ResourceLoadingMode::IMMEDIATE);
                if (texture)
                {
                    quadRenderer->setDiffuseMap(texture);
                    textureMode = SPK::TEXTURE_MODE_2D;
                }
            }

            quadRenderer->setTexturingMode(textureMode);

            bool faceCullEnabled = jsonGetValueWithDefault(groupJson["face_cull_enabled"], true);
            quadRenderer->enableFaceCulling(faceCullEnabled);

            renderer = quadRenderer;
        }
        else if (rendererType == "line")
        {
            renderer = particlesys::LineParticleSystemRenderer::create(100.0f, 100.0f);
        }
        else
        {
            base::glog << "Failed to init particle system: unknown renderer type. Crashing." << base::logwarn;
        }

        // FIXME: can share renderer.
        renderer->enableRenderingOption(SPK::RENDERING_OPTION_DEPTH_WRITE, depthWrite);
        renderer->m_pass->enableDepthTest(depthTest);
        renderer->setBlendMode(spkBlending);

        // Create a group.
        auto particlesGroup = SPK::Group::create(maxParticles);
        particlesGroup->setName(groupName);
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

        // Add modifiers.
        parseSparkModifiers(particlesGroup, groupJson["Modifiers"]);

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
