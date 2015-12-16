#include "ParticleSystemComponentSerializer.h"

#include <components/ParticleSystemComponent.h>
#include <utils/JsonUtils.h>
#include <utils/Utils.h>
#include <particlesys/impl/SparkInterface.h>
#include <render/Texture2D.h>
#include <render/RenderOperation.h>
#include <render/RenderPass.h>
#include <base/EngineController.h>
#include <resources/ResourceManager.h>
#include <resources/ResourceFactory.h>

namespace df3d { namespace component_serializers {

using namespace particlesys_impl;

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
        glog << "Failed to parse spark color interpolator. Empty data field" << logwarn;
        return SPK_NULL_REF;
    }

    auto typeStr = interpolatorJson["type"].asString();
    if (typeStr == "simple")
        return parseSparkSimpleColorInterpolator(dataJson);
    else if (typeStr == "graph")
        return parseSparkGraphColorInterpolator(dataJson);

    glog << "Unknown spark color interpolator type" << typeStr << logwarn;
    return SPK_NULL_REF;
}

SPK::Ref<SPK::FloatDefaultInitializer> parseSparkDefaultInitializer(const Json::Value &dataJson)
{
    float value = 0.0f;
    dataJson["value"] >> value;

    return SPK::FloatDefaultInitializer::create(value);
}

SPK::Ref<SPK::FloatRandomInitializer> parseSparkRandomInitializer(const Json::Value &dataJson)
{
    float minValue = 0.0f, maxValue = 0.0f;
    dataJson["minValue"] >> minValue;
    dataJson["maxValue"] >> maxValue;   // NOTE: spk_random generates random number in interval [a, b)

    return SPK::FloatRandomInitializer::create(minValue, maxValue);
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
        glog << "Failed to parse spark float interpolator. Empty data field" << logwarn;
        return SPK_NULL_REF;
    }

    auto typeStr = interpolatorJson["type"].asString();
    if (typeStr == "simple")
        return parseSparkSimpleInterpolator(dataJson);
    else if (typeStr == "random")
        return parseSparkRandomInterpolator(dataJson);
    else if (typeStr == "graph")
        return parseSparkGraphInterpolator(dataJson);
    else if (typeStr == "defaultInitializer")
        return parseSparkDefaultInitializer(dataJson);
    else if (typeStr == "randomInitializer")
        return parseSparkRandomInitializer(dataJson);

    glog << "Unknown spark float interpolator type" << typeStr << logwarn;
    return SPK_NULL_REF;
}

SPK::Ref<SPK::Emitter> parseSparkEmitter(const Json::Value &emitterJson)
{
    const auto &zoneJson = emitterJson["Zone"];
    if (zoneJson.empty())
    {
        glog << "Emitter has no zone" << logwarn;
        return SPK_NULL_REF;
    }

    std::string zoneType = zoneJson["type"].asString();
    SPK::Ref<SPK::Zone> zone = SPK_NULL_REF;

    // Determine zone type.
    if (zoneType == "Point")
    {
        glm::vec3 position;
        zoneJson["position"] >> position;
        zone = SPK::Point::create(glmToSpk(position));
    }
    else if (zoneType == "Box")
    {
        glm::vec3 position, dimension(1.0f, 1.0f, 1.0f), front(0.0, 0.0, 1.0f), up(0.0f, 1.0f, 0.0f);

        zoneJson["position"] >> position;
        zoneJson["dimension"] >> dimension;
        zoneJson["front"] >> front;
        zoneJson["up"] >> up;

        zone = SPK::Box::create(glmToSpk(position), glmToSpk(dimension), glmToSpk(front), glmToSpk(up));
    }
    else if (zoneType == "Cylinder")
    {
        glm::vec3 position, axis(0.0f, 1.0f, 0.0f);
        float radius = 1.0f, height = 1.0f;

        zoneJson["position"] >> position;
        zoneJson["axis"] >> axis;
        zoneJson["radius"] >> radius;
        zoneJson["height"] >> height;

        zone = SPK::Cylinder::create(glmToSpk(position), height, radius, glmToSpk(axis));
    }
    else if (zoneType == "Plane")
    {
        glm::vec3 position, normal(0.0f, 1.0f, 0.0f);

        zoneJson["position"] >> position;
        zoneJson["normal"] >> normal;

        zone = SPK::Plane::create(glmToSpk(position), glmToSpk(normal));
    }
    else if (zoneType == "Ring")
    {
        glm::vec3 position, normal(0.0f, 1.0f, 0.0f);
        float minRadius = 0.0f, maxRadius = 1.0f;

        zoneJson["position"] >> position;
        zoneJson["normal"] >> normal;
        zoneJson["minRadius"] >> minRadius;
        zoneJson["maxRadius"] >> maxRadius;

        zone = SPK::Ring::create(glmToSpk(position), glmToSpk(normal), minRadius, maxRadius);
    }
    else if (zoneType == "Sphere")
    {
        glm::vec3 position;
        float radius = 1.0f;

        zoneJson["position"] >> position;
        zoneJson["radius"] >> radius;

        zone = SPK::Sphere::create(glmToSpk(position), radius);
    }
    else
    {
        glog << "Unknown zone type" << zoneType << logwarn;
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

        emitter = SPK::StraightEmitter::create(glmToSpk(dir));
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

        emitter = SPK::SphericEmitter::create(glmToSpk(dir), glm::radians(angleA), glm::radians(angleB));
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
        glog << "Unknown emitter type" << emitterType << logwarn;
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
            df3d::glog << "Unknown particle system modifier" << type << df3d::logwarn;
        }
    }
}

SPK::Ref<ParticleSystemRenderer> createRenderer(const Json::Value &rendererJson)
{
    if (rendererJson.empty())
    {
        df3d::glog << "No renderer description in JSON particle system. Crashing" << df3d::logwarn;
        return SPK_NULL_REF;
    }

    std::string rendererType("quad");
    rendererJson["type"] >> rendererType;

    // Create particle system renderer.
    SPK::Ref<ParticleSystemRenderer> renderer;
    if (rendererType == "quad")
    {
        float scaleX = 1.0f, scaleY = 1.0f;
        rendererJson["quadScaleX"] >> scaleX;
        rendererJson["quadScaleY"] >> scaleY;

        auto quadRenderer = QuadParticleSystemRenderer::create(scaleX, scaleY);

        // Setup special renderer params.
        if (!rendererJson["orientation"].empty())
        {
            auto orientationStr = rendererJson["orientation"].asString();
            auto found = StringToSparkDirection.find(orientationStr);
            if (found != StringToSparkDirection.end())
                quadRenderer->setOrientation(found->second);
            else
                glog << "Unknown spark particle system orientation" << logwarn;
        }

        std::string pathToTexture;
        rendererJson["texture"] >> pathToTexture;

        SPK::TextureMode textureMode = SPK::TEXTURE_MODE_NONE;
        if (!pathToTexture.empty())
        {
            auto texture = svc().resourceManager().getFactory().createTexture(pathToTexture, ResourceLoadingMode::ASYNC);
            if (texture)
            {
                quadRenderer->setDiffuseMap(texture);
                textureMode = SPK::TEXTURE_MODE_2D;
            }
        }

        quadRenderer->setTexturingMode(textureMode);

        bool faceCullEnabled = true;
        rendererJson["face_cull_enabled"] >> faceCullEnabled;
        quadRenderer->enableFaceCulling(faceCullEnabled);

        int atlasDimensionX = 1, atlasDimensionY = 1;
        rendererJson["atlasDimensionX"] >> atlasDimensionX;
        rendererJson["atlasDimensionY"] >> atlasDimensionY;

        quadRenderer->setAtlasDimensions(atlasDimensionX, atlasDimensionY);

        renderer = quadRenderer;
    }
    else if (rendererType == "line")
    {
        renderer = LineParticleSystemRenderer::create(100.0f, 100.0f);
    }
    else
    {
        glog << "Failed to init particle system: unknown renderer type. Crashing." << logwarn;
        return SPK_NULL_REF;
    }

    // Get common renderer params.
    bool depthTest = true, depthWrite = true;
    std::string blending = "none";

    rendererJson["depthTest"] >> depthTest;
    rendererJson["depthWrite"] >> depthWrite;
    rendererJson["blending"] >> blending;

    auto spkBlending = SPK::BLEND_MODE_NONE;
    if (blending == "none")
        spkBlending = SPK::BLEND_MODE_NONE;
    else if (blending == "add")
        spkBlending = SPK::BLEND_MODE_ADD;
    else if (blending == "alpha")
        spkBlending = SPK::BLEND_MODE_ALPHA;
    else
        glog << "Unknown blending mode" << blending << logwarn;

    // FIXME: can share renderer.
    renderer->enableRenderingOption(SPK::RENDERING_OPTION_DEPTH_WRITE, depthWrite);
    renderer->m_pass->enableDepthTest(depthTest);
    renderer->setBlendMode(spkBlending);

    return renderer;
}

shared_ptr<NodeComponent> ParticleSystemComponentSerializer::fromJson(const Json::Value &root)
{
    const auto &groupsJson = root["groups"];
    if (groupsJson.empty())
    {
        glog << "Failed to parse particle system configs. Groups node wasn't found" << logwarn;
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
                glog << "Failed to parse particle system group emitter" << logwarn;
        }

        if (emitters.empty())
        {
            glog << "Particle system group has no emitters" << logwarn;
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

        // Create a group.
        auto particlesGroup = SPK::Group::create(maxParticles);
        particlesGroup->setName(groupName);
        particlesGroup->enableSorting(enableSorting);
        particlesGroup->setRenderer(createRenderer(groupJson["Renderer"]));
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
                glog << "Unknown spark param interpolator" << key << logwarn;
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
        glog << "Particle system has no groups" << logwarn;
        return nullptr;
    }

    auto result = make_shared<ParticleSystemComponent>();

    result->setWorldTransformed(utils::json::getOrDefault(root["worldTransformed"], true));
    result->setSystemLifeTime(utils::json::getOrDefault(root["systemLifeTime"], -1.0f));

    for (auto group : systemGroups)
        result->addSPKGroup(group);

    result->initializeSPK();

    return result;
}

Json::Value ParticleSystemComponentSerializer::toJson(shared_ptr<NodeComponent> component)
{
    assert(false);
    // TODO:
    return Json::Value();
}

} }
