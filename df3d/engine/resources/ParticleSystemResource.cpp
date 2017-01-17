#include "ParticleSystemResource.h"

#include <df3d/lib/JsonUtils.h>
#include <df3d/lib/Utils.h>
#include <df3d/engine/EngineController.h>
#include <df3d/engine/resources/ResourceManager.h>
#include <df3d/engine/resources/TextureResource.h>
#include <df3d/engine/particlesys/SparkCommon.h>
#include <df3d/engine/particlesys/SparkQuadRenderer.h>

namespace df3d {

const std::unordered_map<Id, SPK::Param> StringToSparkParam =
{
    { Id("angle"), SPK::PARAM_ANGLE },
    { Id("mass"), SPK::PARAM_MASS },
    { Id("rotation_speed"), SPK::PARAM_ROTATION_SPEED },
    { Id("scale"), SPK::PARAM_SCALE },
    { Id("texture_index"), SPK::PARAM_TEXTURE_INDEX }
};

const std::unordered_map<Id, SPK::OrientationPreset> StringToSparkDirection =
{
    { Id("direction_aligned"), SPK::DIRECTION_ALIGNED },
    { Id("camera_plane_aligned"), SPK::CAMERA_PLANE_ALIGNED },
    { Id("camera_point_aligned"), SPK::CAMERA_POINT_ALIGNED },
    { Id("around_axis"), SPK::AROUND_AXIS },
    { Id("towards_point"), SPK::TOWARDS_POINT },
    { Id("fixed_orientation"), SPK::FIXED_ORIENTATION },
};

static SPK::Color ToSpkColor(glm::vec4 color)
{
    color *= 255.0f;
    return SPK::Color((int)color.r, (int)color.g, (int)color.b, (int)color.a);
}

static SPK::Ref<SPK::ColorSimpleInterpolator> ParseSparkSimpleColorInterpolator(const rapidjson::Value &dataJson)
{
    auto birthColor = JsonUtils::get(dataJson, "birth", glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
    auto deathColor = JsonUtils::get(dataJson, "death", glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));

    return SPK::ColorSimpleInterpolator::create(ToSpkColor(birthColor), ToSpkColor(deathColor));
}

static SPK::Ref<SPK::ColorGraphInterpolator> ParseSparkGraphColorInterpolator(const rapidjson::Value &dataJson)
{
    auto result = SPK::ColorGraphInterpolator::create();

    for (const auto &entryJson : dataJson.GetArray())
    {
        auto x = JsonUtils::get(entryJson, "x", 0.0f);
        auto y = JsonUtils::get(entryJson, "y", glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));

        result->addEntry(x, ToSpkColor(y));
    }

    return result;
}

static SPK::Ref<SPK::ColorInterpolator> ParseSparkColorInterpolator(const rapidjson::Value &interpolatorJson)
{
    if (interpolatorJson.IsNull())
        return SPK_NULL_REF;

    if (!interpolatorJson.HasMember("data") || !interpolatorJson.HasMember("type"))
    {
        DFLOG_WARN("Failed to parse spark color interpolator. Invalid 'data' or 'type' field");
        return SPK_NULL_REF;
    }

    auto typeStr = Id(interpolatorJson["type"].GetString());
    if (typeStr == Id("simple"))
        return ParseSparkSimpleColorInterpolator(interpolatorJson["data"]);
    else if (typeStr == Id("graph"))
        return ParseSparkGraphColorInterpolator(interpolatorJson["data"]);

    DFLOG_WARN("Unknown spark color interpolator type: %s", interpolatorJson["type"].GetString());
    return SPK_NULL_REF;
}

static SPK::Ref<SPK::FloatDefaultInitializer> ParseSparkDefaultInitializer(const rapidjson::Value &dataJson)
{
    auto value = JsonUtils::get(dataJson, "value", 0.0f);

    return SPK::FloatDefaultInitializer::create(value);
}

static SPK::Ref<SPK::FloatRandomInitializer> ParseSparkRandomInitializer(const rapidjson::Value &dataJson)
{
    // NOTE: spk_random generates random number in interval [a, b)
    auto minValue = JsonUtils::get(dataJson, "minValue", 0.0f);
    auto maxValue = JsonUtils::get(dataJson, "maxValue", 0.0f);

    return SPK::FloatRandomInitializer::create(minValue, maxValue);
}

static SPK::Ref<SPK::FloatSimpleInterpolator> ParseSparkSimpleInterpolator(const rapidjson::Value &dataJson)
{
    auto birthValue = JsonUtils::get(dataJson, "birth", 0.0f);
    auto deathValue = JsonUtils::get(dataJson, "death", 0.0f);

    return SPK::FloatSimpleInterpolator::create(birthValue, deathValue);
}

static SPK::Ref<SPK::FloatRandomInterpolator> ParseSparkRandomInterpolator(const rapidjson::Value &dataJson)
{
    auto birthRange = dataJson["birth"].GetArray();
    auto deathRange = dataJson["death"].GetArray();

    float minBirth = birthRange[0].GetFloat();
    float maxBirth = birthRange[1].GetFloat();
    float minDeath = deathRange[0].GetFloat();
    float maxDeath = deathRange[1].GetFloat();

    return SPK::FloatRandomInterpolator::create(minBirth, maxBirth, minDeath, maxDeath);
}

static SPK::Ref<SPK::FloatGraphInterpolator> ParseSparkGraphInterpolator(const rapidjson::Value &dataJson)
{
    auto result = SPK::FloatGraphInterpolator::create();

    for (const auto &entryJson : dataJson.GetArray())
    {
        auto x = JsonUtils::get(entryJson, "x", 0.0f);
        auto y = JsonUtils::get(entryJson, "y", 0.0f);

        result->addEntry(x, y);
    }

    return result;
}

static SPK::Ref<SPK::FloatInterpolator> ParseSparkParamInterpolator(const rapidjson::Value &interpolatorJson)
{
    if (interpolatorJson.IsNull())
        return SPK_NULL_REF;

    if (!interpolatorJson.HasMember("data") || !interpolatorJson.HasMember("type"))
    {
        DFLOG_WARN("Failed to parse spark float interpolator. Invalid 'data' or 'type' field");
        return SPK_NULL_REF;
    }

    const auto &dataJson = interpolatorJson["data"];
    auto typeStr = Id(interpolatorJson["type"].GetString());
    if (typeStr == Id("simple"))
        return ParseSparkSimpleInterpolator(dataJson);
    else if (typeStr == Id("random"))
        return ParseSparkRandomInterpolator(dataJson);
    else if (typeStr == Id("graph"))
        return ParseSparkGraphInterpolator(dataJson);
    else if (typeStr == Id("defaultInitializer"))
        return ParseSparkDefaultInitializer(dataJson);
    else if (typeStr == Id("randomInitializer"))
        return ParseSparkRandomInitializer(dataJson);

    DFLOG_WARN("Unknown spark float interpolator type: %s", interpolatorJson["type"].GetString());
    return SPK_NULL_REF;
}

static SPK::Ref<SPK::Emitter> ParseSparkEmitter(const rapidjson::Value &emitterJson)
{
    if (!emitterJson.HasMember("Zone") || !emitterJson.HasMember("type"))
    {
        DFLOG_WARN("Emitter has no zone");
        return SPK_NULL_REF;
    }

    SPK::Ref<SPK::Zone> zone = SPK_NULL_REF;
    SPK::Ref<SPK::Emitter> emitter = SPK_NULL_REF;

    {
        const auto &zoneJson = emitterJson["Zone"];
        Id zoneType = Id(zoneJson["type"].GetString());

        // Determine zone type.
        if (zoneType == Id("Point"))
        {
            auto position = JsonUtils::get(zoneJson, "position", glm::vec3());
            zone = SPK::Point::create(glmToSpk(position));
        }
        else if (zoneType == Id("Box"))
        {
            auto position = JsonUtils::get(zoneJson, "position", glm::vec3(0.0f));
            auto dimension = JsonUtils::get(zoneJson, "dimension", glm::vec3(1.0f, 1.0f, 1.0f));
            auto front = JsonUtils::get(zoneJson, "front", glm::vec3(0.0, 0.0, 1.0f));
            auto up = JsonUtils::get(zoneJson, "up", glm::vec3(0.0f, 1.0f, 0.0f));

            zone = SPK::Box::create(glmToSpk(position),
                                    glmToSpk(dimension),
                                    glmToSpk(front),
                                    glmToSpk(up));
        }
        else if (zoneType == Id("Cylinder"))
        {
            auto position = JsonUtils::get(zoneJson, "position", glm::vec3());
            auto axis = JsonUtils::get(zoneJson, "axis", glm::vec3(0.0f, 1.0f, 0.0f));
            auto radius = JsonUtils::get(zoneJson, "radius", 1.0f);
            auto height = JsonUtils::get(zoneJson, "height", 1.0f);

            zone = SPK::Cylinder::create(glmToSpk(position),
                                         height,
                                         radius,
                                         glmToSpk(axis));
        }
        else if (zoneType == Id("Plane"))
        {
            auto position = JsonUtils::get(zoneJson, "position", glm::vec3());
            auto normal = JsonUtils::get(zoneJson, "normal", glm::vec3(0.0f, 1.0f, 0.0f));

            zone = SPK::Plane::create(glmToSpk(position),
                                      glmToSpk(normal));
        }
        else if (zoneType == Id("Ring"))
        {
            auto position = JsonUtils::get(zoneJson, "position", glm::vec3());
            auto normal = JsonUtils::get(zoneJson, "normal", glm::vec3(0.0f, 1.0f, 0.0f));
            auto minRadius = JsonUtils::get(zoneJson, "minRadius", 0.0f);
            auto maxRadius = JsonUtils::get(zoneJson, "maxRadius", 1.0f);

            zone = SPK::Ring::create(glmToSpk(position),
                                     glmToSpk(normal),
                                     minRadius,
                                     maxRadius);
        }
        else if (zoneType == Id("Sphere"))
        {
            auto position = JsonUtils::get(zoneJson, "position", glm::vec3());
            auto radius = JsonUtils::get(zoneJson, "radius", 1.0f);

            zone = SPK::Sphere::create(glmToSpk(position), radius);
        }
        else
        {
            DFLOG_WARN("Unknown zone type %s", zoneJson["type"].GetString());
            return SPK_NULL_REF;
        }
    }

    {
        Id emitterType = Id(emitterJson["type"].GetString());

        // Determine emitter type.
        if (emitterType == Id("Random"))
        {
            emitter = SPK::RandomEmitter::create();
        }
        else if (emitterType == Id("Straight"))
        {
            auto dir = JsonUtils::get(emitterJson, "direction", glm::vec3(0.0f, 0.0f, -1.0f));

            emitter = SPK::StraightEmitter::create(glmToSpk(dir));
        }
        else if (emitterType == Id("Static"))
        {
            emitter = SPK::StaticEmitter::create();
        }
        else if (emitterType == Id("Spheric"))
        {
            auto dir = JsonUtils::get(emitterJson, "direction", glm::vec3(0.0f, 0.0f, -1.0f));
            auto angleA = JsonUtils::get(emitterJson, "angleA", 0.0f);
            auto angleB = JsonUtils::get(emitterJson, "angleB", 0.0f);

            emitter = SPK::SphericEmitter::create(glmToSpk(dir),
                                                  glm::radians(angleA),
                                                  glm::radians(angleB));
        }
        else if (emitterType == Id("Normal"))
        {
            emitter = SPK::NormalEmitter::create();

            auto inverted = JsonUtils::get(emitterJson, "inverted", false);

            ((SPK::NormalEmitter*)emitter.get())->setInverted(inverted);
        }
        else
        {
            DFLOG_WARN("Unknown emitter type %s", emitterJson["type"].GetString());
        }
    }

    // Check for valid emitter.
    if (!emitter)
        return SPK_NULL_REF;

    // Init emitter.
    auto flow = JsonUtils::get(emitterJson, "flow", 0.0f);
    auto minForce = JsonUtils::get(emitterJson, "minForce", 0.0f);
    auto maxForce = JsonUtils::get(emitterJson, "maxForce", 0.0f);
    auto tank = JsonUtils::get(emitterJson, "tank", -1);
    auto fullGen = JsonUtils::get(emitterJson, "generateOnZoneBorders", false);

    emitter->setTank(tank);
    emitter->setFlow(flow);
    emitter->setForce(minForce, maxForce);
    emitter->setZone(zone, !fullGen);

    return emitter;
}

static void ParseSparkModifiers(SPK::Ref<SPK::Group> group, const rapidjson::Value &modifiersJson)
{
    for (const auto &modifierJson : modifiersJson.GetArray())
    {
        auto type = Id(modifierJson["type"].GetString());

        if (type == Id("gravity"))
        {
            auto val = JsonUtils::get(modifierJson, "value", glm::vec3());
            group->addModifier(SPK::Gravity::create({ val.x, val.y, val.z }));
        }
        else if (type == Id("friction"))
        {
            auto val = JsonUtils::get(modifierJson, "value", 0.0f);
            group->addModifier(SPK::Friction::create(val));
        }
        else if (type == Id("random_force"))
        {
            auto minForce = JsonUtils::get(modifierJson, "minForce", glm::vec3());
            auto maxForce = JsonUtils::get(modifierJson, "maxForce", glm::vec3());
            auto minPeriod = JsonUtils::get(modifierJson, "minPeriod", 1.0f);
            auto maxPeriod = JsonUtils::get(modifierJson, "maxPeriod", 1.0f);

            group->addModifier(SPK::RandomForce::create(glmToSpk(minForce),
                                                        glmToSpk(maxForce),
                                                        minPeriod,
                                                        maxPeriod));
        }
        else
        {
            DFLOG_WARN("Unknown particle system modifier %s", modifierJson["type"].GetString());
        }
    }
}

static SPK::Ref<ParticleSystemRenderer> CreateRenderer(const rapidjson::Value &rendererJson)
{
    if (rendererJson.IsNull())
    {
        DFLOG_WARN("No renderer description in JSON particle system.");
        return SPK_NULL_REF;
    }

    Id rendererType("quad");
    if (rendererJson.HasMember("type"))
        rendererType = Id(rendererJson["type"].GetString());

    // Create particle system renderer.
    SPK::Ref<ParticleSystemRenderer> renderer;
    if (rendererType == Id("quad"))
    {
        auto scaleX = JsonUtils::get(rendererJson, "quadScaleX", 1.0f);
        auto scaleY = JsonUtils::get(rendererJson, "quadScaleY", 1.0f);

        auto quadRenderer = QuadParticleSystemRenderer::create(scaleX, scaleY);

        // Setup special renderer params.
        if (rendererJson.HasMember("orientation"))
        {
            auto orientationStr = Id(rendererJson["orientation"].GetString());
            auto found = StringToSparkDirection.find(orientationStr);
            if (found != StringToSparkDirection.end())
                quadRenderer->setOrientation(found->second);
            else
                DFLOG_WARN("Unknown spark particle system orientation %s", rendererJson["orientation"].GetString());
        }

        std::string pathToTexture = JsonUtils::get(rendererJson, "texture", std::string(""));

        SPK::TextureMode textureMode = SPK::TEXTURE_MODE_NONE;
        if (!pathToTexture.empty())
        {
            auto texture = svc().resourceManager().getResource<TextureResource>(Id(pathToTexture.c_str()));
            if (texture)
            {
                quadRenderer->setDiffuseMap(texture->handle);
                textureMode = SPK::TEXTURE_MODE_2D;
            }
        }

        quadRenderer->setTexturingMode(textureMode);

        bool faceCullEnabled = JsonUtils::get(rendererJson, "face_cull_enabled", true);
        quadRenderer->enableFaceCulling(faceCullEnabled);

        int atlasDimensionX = JsonUtils::get(rendererJson, "atlasDimensionX", 1);
        int atlasDimensionY = JsonUtils::get(rendererJson, "atlasDimensionY", 1);

        quadRenderer->setAtlasDimensions(atlasDimensionX, atlasDimensionY);

        renderer = quadRenderer;
    }
    else if (rendererType == Id("line"))
    {
        DF3D_ASSERT_MESS(false, "not implemented");
        //renderer = LineParticleSystemRenderer::create(100.0f, 100.0f);
    }
    else
    {
        DFLOG_WARN("Failed to init particle system: unknown renderer type.");
        return SPK_NULL_REF;
    }

    // Get common renderer params.
    Id blending = Id("none");
    auto depthTest = JsonUtils::get(rendererJson, "depthTest", true);
    auto depthWrite = JsonUtils::get(rendererJson, "depthWrite", true);

    if (rendererJson.HasMember("blending"))
        blending = Id(rendererJson["blending"].GetString());

    auto spkBlending = SPK::BLEND_MODE_NONE;
    if (blending == Id("none"))
        spkBlending = SPK::BLEND_MODE_NONE;
    else if (blending == Id("add"))
        spkBlending = SPK::BLEND_MODE_ADD;
    else if (blending == Id("alpha"))
        spkBlending = SPK::BLEND_MODE_ALPHA;
    else
        DFLOG_WARN("Unknown blending mode");

    // FIXME: can share renderer.
    renderer->enableRenderingOption(SPK::RENDERING_OPTION_DEPTH_WRITE, depthWrite);
    renderer->m_pass.depthTest = depthTest;
    renderer->setBlendMode(spkBlending);

    return renderer;
}

static SPK::Ref<SPK::System> CreateSpkSystem(const rapidjson::Value &root)
{
    if (!root.HasMember("groups"))
    {
        DFLOG_WARN("Failed to parse particle system configs. Groups node wasn't found");
        return SPK_NULL_REF;
    }

    std::vector<SPK::Ref<SPK::Group>> systemGroups;

    // Parse all particle system groups.
    const auto &groupsJson = root["groups"];
    for (const auto &groupJson : groupsJson.GetArray())
    {
        // Parse group emitters.
        std::vector<SPK::Ref<SPK::Emitter>> emitters;

        if (groupJson.HasMember("Emitters"))
        {
            const auto &emittersJson = groupJson["Emitters"];
            for (const auto &emitterJson : emittersJson.GetArray())
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
        }

        // Get group params.
        auto groupName = JsonUtils::get(groupJson, "name", std::string());
        auto enableSorting = JsonUtils::get(groupJson, "enableSorting", false);
        auto maxParticles = JsonUtils::get(groupJson, "maxParticles", 100);
        auto minLifeTime = JsonUtils::get(groupJson, "minLifeTime", 1.0f);
        auto maxLifeTime = JsonUtils::get(groupJson, "maxLifeTime", 1.0f);
        auto immortal = JsonUtils::get(groupJson, "immortal", false);
        auto still = JsonUtils::get(groupJson, "still", false);
        auto radius = JsonUtils::get(groupJson, "radius", 1.0f);

        // Create a group.
        auto particlesGroup = SPK::Group::create(maxParticles);
        particlesGroup->setName(groupName);
        particlesGroup->enableSorting(enableSorting);
        particlesGroup->setLifeTime(minLifeTime, maxLifeTime);
        particlesGroup->setImmortal(immortal);
        particlesGroup->setStill(still);
        particlesGroup->setRadius(radius);

        if (groupJson.HasMember("Renderer"))
            particlesGroup->setRenderer(CreateRenderer(groupJson["Renderer"]));

        // Add emitters.
        for (auto emitter : emitters)
            particlesGroup->addEmitter(emitter);

        // Add color interpolator if any.
        if (groupJson.HasMember("ColorInterpolator"))
        {
            if (auto interpolator = ParseSparkColorInterpolator(groupJson["ColorInterpolator"]))
                particlesGroup->setColorInterpolator(interpolator);
        }

        // Add param interpolators.
        if (groupJson.HasMember("ParamInterpolators"))
        {
            const auto &paramInterpolatorsJson = groupJson["ParamInterpolators"];

            for (const auto &kv : paramInterpolatorsJson.GetObject())
            {
                auto key = Id(kv.name.GetString());
                if (!utils::contains_key(StringToSparkParam, key))
                {
                    DFLOG_WARN("Unknown spark param interpolator %s", kv.name.GetString());
                    continue;
                }

                particlesGroup->setParamInterpolator(StringToSparkParam.find(key)->second, ParseSparkParamInterpolator(kv.value));
            }
        }

        // Add modifiers.
        if (groupJson.HasMember("Modifiers"))
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

    auto worldTransformed = JsonUtils::get(root, "worldTransformed", true);
    auto systemLifeTime = JsonUtils::get(root, "systemLifeTime", -1.0f);

    result->worldTransformed = worldTransformed;
    result->lifetime = systemLifeTime;

    return result;
}

void ParticleSystemHolder::listDependencies(ResourceDataSource &dataSource, std::vector<std::string> &outDeps)
{
    auto root = JsonUtils::fromFile(dataSource);
    if (root.IsNull())
        return;

    if (!root.HasMember("groups"))
        return;

    const auto &jsonGroups = root["groups"];
    for (const auto &jsonGroup : jsonGroups.GetArray())
    {
        if (jsonGroup.HasMember("Renderer"))
        {
            const auto &renderJson = jsonGroup["Renderer"];
            if (renderJson.HasMember("texture"))
            {
                std::string path = renderJson["texture"].GetString();
                outDeps.push_back(path);
            }
        }
    }
}

bool ParticleSystemHolder::decodeStartup(ResourceDataSource &dataSource, Allocator &allocator)
{
    auto root = JsonUtils::fromFile(dataSource);
    if (root.IsNull())
        return false;

    m_root = MAKE_NEW(allocator, rapidjson::Document)(std::move(root));

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
