#include "ParticleSystemResource.h"

#include <df3d/lib/JsonUtils.h>
#include <df3d/lib/Utils.h>
#include <df3d/engine/EngineController.h>
#include <df3d/engine/resources/ResourceManager.h>
#include <df3d/engine/resources/TextureResource.h>
#include <df3d/engine/particlesys/SparkCommon.h>
#include <df3d/engine/particlesys/SparkQuadRenderer.h>

namespace df3d {

const std::map<std::string, SPK::Param> StringToSparkParam =
{
    { "angle", SPK::PARAM_ANGLE },
    { "mass", SPK::PARAM_MASS },
    { "rotation_speed", SPK::PARAM_ROTATION_SPEED },
    { "scale", SPK::PARAM_SCALE },
    { "texture_index", SPK::PARAM_TEXTURE_INDEX }
};

const std::map<std::string, SPK::OrientationPreset> StringToSparkDirection =
{
    { "direction_aligned", SPK::DIRECTION_ALIGNED },
    { "camera_plane_aligned", SPK::CAMERA_PLANE_ALIGNED },
    { "camera_point_aligned", SPK::CAMERA_POINT_ALIGNED },
    { "around_axis", SPK::AROUND_AXIS },
    { "towards_point", SPK::TOWARDS_POINT },
    { "fixed_orientation", SPK::FIXED_ORIENTATION },
};

static SPK::Color ToSpkColor(glm::vec4 color)
{
    color *= 255.0f;
    return SPK::Color((int)color.r, (int)color.g, (int)color.b, (int)color.a);
}

static SPK::Ref<SPK::ColorSimpleInterpolator> ParseSparkSimpleColorInterpolator(const Json::Value &dataJson)
{
    glm::vec4 birthColor(0.0f, 0.0f, 0.0f, 1.0f);
    glm::vec4 deathColor(0.0f, 0.0f, 0.0f, 1.0f);

    dataJson["birth"] >> birthColor;
    dataJson["death"] >> deathColor;

    return SPK::ColorSimpleInterpolator::create(ToSpkColor(birthColor), ToSpkColor(deathColor));
}

static SPK::Ref<SPK::ColorGraphInterpolator> ParseSparkGraphColorInterpolator(const Json::Value &dataJson)
{
    auto result = SPK::ColorGraphInterpolator::create();

    for (const auto &entryJson : dataJson)
    {
        float x = 0.0f;
        glm::vec4 y(0.0f, 0.0f, 0.0f, 1.0f);

        entryJson["x"] >> x;
        entryJson["y"] >> y;

        result->addEntry(x, ToSpkColor(y));
    }

    return result;
}

static SPK::Ref<SPK::ColorInterpolator> ParseSparkColorInterpolator(const Json::Value &interpolatorJson)
{
    if (interpolatorJson.empty())
        return SPK_NULL_REF;

    const auto &dataJson = interpolatorJson["data"];
    if (dataJson.empty())
    {
        DFLOG_WARN("Failed to parse spark color interpolator. Empty data field");
        return SPK_NULL_REF;
    }

    auto typeStr = interpolatorJson["type"].asString();
    if (typeStr == "simple")
        return ParseSparkSimpleColorInterpolator(dataJson);
    else if (typeStr == "graph")
        return ParseSparkGraphColorInterpolator(dataJson);

    DFLOG_WARN("Unknown spark color interpolator type: %s", typeStr.c_str());
    return SPK_NULL_REF;
}

static SPK::Ref<SPK::FloatDefaultInitializer> ParseSparkDefaultInitializer(const Json::Value &dataJson)
{
    float value = 0.0f;
    dataJson["value"] >> value;

    return SPK::FloatDefaultInitializer::create(value);
}

static SPK::Ref<SPK::FloatRandomInitializer> ParseSparkRandomInitializer(const Json::Value &dataJson)
{
    float minValue = 0.0f, maxValue = 0.0f;
    dataJson["minValue"] >> minValue;
    dataJson["maxValue"] >> maxValue;   // NOTE: spk_random generates random number in interval [a, b)

    return SPK::FloatRandomInitializer::create(minValue, maxValue);
}

static SPK::Ref<SPK::FloatSimpleInterpolator> ParseSparkSimpleInterpolator(const Json::Value &dataJson)
{
    float birthValue = 0.0f;
    float deathValue = 0.0f;

    dataJson["birth"] >> birthValue;
    dataJson["death"] >> deathValue;

    return SPK::FloatSimpleInterpolator::create(birthValue, deathValue);
}

static SPK::Ref<SPK::FloatRandomInterpolator> ParseSparkRandomInterpolator(const Json::Value &dataJson)
{
    const auto &birthRange = dataJson["birth"];
    const auto &deathRange = dataJson["death"];

    float minBirth = birthRange[0].asFloat();
    float maxBirth = birthRange[1].asFloat();
    float minDeath = deathRange[0].asFloat();
    float maxDeath = deathRange[1].asFloat();

    return SPK::FloatRandomInterpolator::create(minBirth, maxBirth, minDeath, maxDeath);
}

static SPK::Ref<SPK::FloatGraphInterpolator> ParseSparkGraphInterpolator(const Json::Value &dataJson)
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

static SPK::Ref<SPK::FloatInterpolator> ParseSparkParamInterpolator(const Json::Value &interpolatorJson)
{
    if (interpolatorJson.empty())
        return SPK_NULL_REF;

    const auto &dataJson = interpolatorJson["data"];
    if (dataJson.empty())
    {
        DFLOG_WARN("Failed to parse spark float interpolator. Empty data field");
        return SPK_NULL_REF;
    }

    auto typeStr = interpolatorJson["type"].asString();
    if (typeStr == "simple")
        return ParseSparkSimpleInterpolator(dataJson);
    else if (typeStr == "random")
        return ParseSparkRandomInterpolator(dataJson);
    else if (typeStr == "graph")
        return ParseSparkGraphInterpolator(dataJson);
    else if (typeStr == "defaultInitializer")
        return ParseSparkDefaultInitializer(dataJson);
    else if (typeStr == "randomInitializer")
        return ParseSparkRandomInitializer(dataJson);

    DFLOG_WARN("Unknown spark float interpolator type: %s", typeStr.c_str());
    return SPK_NULL_REF;
}

static SPK::Ref<SPK::Emitter> ParseSparkEmitter(const Json::Value &emitterJson)
{
    const auto &zoneJson = emitterJson["Zone"];
    if (zoneJson.empty())
    {
        DFLOG_WARN("Emitter has no zone");
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

        zone = SPK::Box::create(glmToSpk(position),
                                glmToSpk(dimension),
                                glmToSpk(front),
                                glmToSpk(up));
    }
    else if (zoneType == "Cylinder")
    {
        glm::vec3 position, axis(0.0f, 1.0f, 0.0f);
        float radius = 1.0f, height = 1.0f;

        zoneJson["position"] >> position;
        zoneJson["axis"] >> axis;
        zoneJson["radius"] >> radius;
        zoneJson["height"] >> height;

        zone = SPK::Cylinder::create(glmToSpk(position),
                                     height,
                                     radius,
                                     glmToSpk(axis));
    }
    else if (zoneType == "Plane")
    {
        glm::vec3 position, normal(0.0f, 1.0f, 0.0f);

        zoneJson["position"] >> position;
        zoneJson["normal"] >> normal;

        zone = SPK::Plane::create(glmToSpk(position),
                                  glmToSpk(normal));
    }
    else if (zoneType == "Ring")
    {
        glm::vec3 position, normal(0.0f, 1.0f, 0.0f);
        float minRadius = 0.0f, maxRadius = 1.0f;

        zoneJson["position"] >> position;
        zoneJson["normal"] >> normal;
        zoneJson["minRadius"] >> minRadius;
        zoneJson["maxRadius"] >> maxRadius;

        zone = SPK::Ring::create(glmToSpk(position),
                                 glmToSpk(normal),
                                 minRadius,
                                 maxRadius);
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
        DFLOG_WARN("Unknown zone type %s", zoneType.c_str());
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

        emitter = SPK::SphericEmitter::create(glmToSpk(dir),
                                              glm::radians(angleA),
                                              glm::radians(angleB));
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
        DFLOG_WARN("Unknown emitter type %s", emitterType.c_str());
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

static void ParseSparkModifiers(SPK::Ref<SPK::Group> group, const Json::Value &modifiersJson)
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
        else if (type == "random_force")
        {
            glm::vec3 minForce, maxForce;
            float minPeriod = 1.0f, maxPeriod = 1.0f;
            modifierJson["minForce"] >> minForce;
            modifierJson["maxForce"] >> maxForce;
            modifierJson["minPeriod"] >> minPeriod;
            modifierJson["maxPeriod"] >> maxPeriod;

            group->addModifier(SPK::RandomForce::create(glmToSpk(minForce),
                                                        glmToSpk(maxForce),
                                                        minPeriod,
                                                        maxPeriod));
        }
        else
        {
            DFLOG_WARN("Unknown particle system modifier %s", type.c_str());
        }
    }
}

static SPK::Ref<ParticleSystemRenderer> CreateRenderer(const Json::Value &rendererJson)
{
    if (rendererJson.empty())
    {
        DFLOG_WARN("No renderer description in JSON particle system. May crash...");
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
                DFLOG_WARN("Unknown spark particle system orientation %s", orientationStr.c_str());
        }

        std::string pathToTexture;
        rendererJson["texture"] >> pathToTexture;

        SPK::TextureMode textureMode = SPK::TEXTURE_MODE_NONE;
        if (!pathToTexture.empty())
        {
            auto texture = svc().resourceManager().getResource<TextureResource>(pathToTexture);
            if (texture)
            {
                quadRenderer->setDiffuseMap(texture->handle);
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
        DF3D_ASSERT_MESS(false, "not implemented");
        //renderer = LineParticleSystemRenderer::create(100.0f, 100.0f);
    }
    else
    {
        DFLOG_WARN("Failed to init particle system: unknown renderer type. Crashing...");
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
        DFLOG_WARN("Unknown blending mode %s", blending.c_str());

    // FIXME: can share renderer.
    renderer->enableRenderingOption(SPK::RENDERING_OPTION_DEPTH_WRITE, depthWrite);
    renderer->m_pass.depthTest = depthTest;
    renderer->setBlendMode(spkBlending);

    return renderer;
}

static SPK::Ref<SPK::System> CreateSpkSystem(const Json::Value &root)
{
    const auto &groupsJson = root["groups"];
    if (groupsJson.empty())
    {
        DFLOG_WARN("Failed to parse particle system configs. Groups node wasn't found");
        return SPK_NULL_REF;
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
            auto emitter = ParseSparkEmitter(emitterJson);
            if (emitter)
                emitters.push_back(emitter);
            else
                DFLOG_WARN("Failed to parse particle system group emitter");
        }

        if (emitters.empty())
        {
            DFLOG_WARN("Particle system group has no emitters");
            continue;
        }

        // Get group params.
        std::string groupName;
        bool enableSorting = false, immortal = false, still = false;
        int maxParticles = 100;
        float minLifeTime = 1.0f, maxLifeTime = 1.0f, radius = 1.0f;

        groupJson["name"] >> groupName;
        groupJson["enableSorting"] >> enableSorting;
        groupJson["maxParticles"] >> maxParticles;
        groupJson["minLifeTime"] >> minLifeTime;
        groupJson["maxLifeTime"] >> maxLifeTime;
        groupJson["immortal"] >> immortal;
        groupJson["still"] >> still;
        groupJson["radius"] >> radius;

        // Create a group.
        auto particlesGroup = SPK::Group::create(maxParticles);
        particlesGroup->setName(groupName);
        particlesGroup->enableSorting(enableSorting);
        particlesGroup->setRenderer(CreateRenderer(groupJson["Renderer"]));
        particlesGroup->setLifeTime(minLifeTime, maxLifeTime);
        particlesGroup->setImmortal(immortal);
        particlesGroup->setStill(still);
        particlesGroup->setRadius(radius);

        // Add emitters.
        for (auto emitter : emitters)
            particlesGroup->addEmitter(emitter);

        // Add color interpolator if any.
        auto colorInterpolatorJson = ParseSparkColorInterpolator(groupJson["ColorInterpolator"]);
        if (colorInterpolatorJson)
            particlesGroup->setColorInterpolator(colorInterpolatorJson);

        // Add param interpolators.
        auto paramInterpolatorsJson = groupJson["ParamInterpolators"];
        for (auto it = paramInterpolatorsJson.begin(); it != paramInterpolatorsJson.end(); it++)
        {
            auto key = it.key().asString();
            if (!utils::contains_key(StringToSparkParam, key))
            {
                DFLOG_WARN("Unknown spark param interpolator %s", key.c_str());
                continue;
            }

            particlesGroup->setParamInterpolator(StringToSparkParam.find(key)->second, ParseSparkParamInterpolator(*it));
        }

        // Add modifiers.
        ParseSparkModifiers(particlesGroup, groupJson["Modifiers"]);

        // Store group.
        systemGroups.push_back(particlesGroup);
    }

    if (systemGroups.empty())
    {
        DFLOG_WARN("Particle system has no groups");
        return SPK_NULL_REF;
    }

    auto result = SPK::System::create(false);

    for (auto group : systemGroups)
        result->addGroup(group);

    result->initialize();

    bool worldTransformed = true;
    float systemLifeTime = -1.0f;

    root["worldTransformed"] >> worldTransformed;
    root["systemLifeTime"] >> systemLifeTime;
    result->worldTransformed = worldTransformed;
    result->lifetime = systemLifeTime;

    return result;
}

bool ParticleSystemHolder::decodeStartup(ResourceDataSource &dataSource, Allocator &allocator)
{
    Json::Value root = JsonUtils::fromFile(dataSource);
    if (root.isNull())
        return false;

    const auto &jsonGroups = root["groups"];
    for (const auto &jsonGroup : jsonGroups)
    {
        if (jsonGroup.isMember("Renderer"))
        {
            const auto &renderJson = jsonGroup["Renderer"];
            if (renderJson.isMember("texture"))
            {
                auto path = renderJson["texture"].asString();
                svc().resourceManager().loadResource(path);
            }
        }
    }

    m_root = MAKE_NEW(allocator, Json::Value)(std::move(root));

    return true;
}

void ParticleSystemHolder::decodeCleanup(Allocator &allocator)
{
    MAKE_DELETE(allocator, m_root);
    m_root = nullptr;
}

bool ParticleSystemHolder::createResource(Allocator &allocator)
{
    m_resource = MAKE_NEW(allocator, ParticleSystemResource)();

    m_resource->spkSystem = CreateSpkSystem(*m_root);

    return m_resource->spkSystem.get() != nullptr;
}

void ParticleSystemHolder::destroyResource(Allocator &allocator)
{
    MAKE_DELETE(allocator, m_resource);
    m_resource = nullptr;
}

}
