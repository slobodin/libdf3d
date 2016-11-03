#include "ScriptBindings.h"

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4244) // possible loss of data
#endif
#include <sqrat/sqrat.h>
#ifdef _MSC_VER
#pragma warning(pop)
#endif

#include <df3d/lib/Utils.h>
#include <df3d/lib/math/MathUtils.h>
#include <df3d/engine/EngineController.h>
#include <df3d/game/Entity.h>
#include <df3d/game/World.h>
#include <df3d/game/TagComponentProcessor.h>
#include <df3d/engine/2d/Sprite2DComponentProcessor.h>
#include <df3d/engine/3d/SceneGraphComponentProcessor.h>
#include <df3d/engine/3d/Camera.h>
#include <df3d/engine/3d/StaticMeshComponentProcessor.h>
#include <df3d/engine/3d/Light.h>
#include <df3d/engine/particlesys/ParticleSystemComponentProcessor.h>
#include <df3d/engine/resources/ResourceManager.h>
#include <df3d/engine/resources/ResourceFileSystem.h>
#include <df3d/engine/resources/ResourceDataSource.h>

using namespace Sqrat;

namespace df3d { namespace script_impl {

inline int random_int_range(int a, int b)
{
    DF3D_ASSERT(a <= b);
    return RandomUtils::randRange(a, b);
}

inline float random_float_range(float a, float b)
{
    DF3D_ASSERT(a <= b);
    return RandomUtils::randRange(a, b);
}

inline int rand()
{
    return RandomUtils::rand();
}

inline void srandReset()
{
    RandomUtils::srand();
}

inline void srand(int seed)
{
    RandomUtils::srand(seed);
}

inline glm::quat slerp(const glm::quat &a, const glm::quat &b, float t)
{
    return glm::slerp(a, b, t);
}

inline glm::quat toQuat(const glm::vec3 &eulerAngles)
{
    return glm::quat(eulerAngles);
}

inline World* df3dWorld()
{
    return &svc().defaultWorld();
}

inline Camera* getWorldCamera()
{
    return svc().defaultWorld().getCamera().get();
}

inline glm::vec2 getScreenSize()
{
    return svc().getScreenSize();
}

inline bool executeFile(const char *filename)
{
    return svc().scripts().doFile(filename);
}

inline Entity createEntity(uint32_t id)
{
    return Entity(id);
}

void bindGlm(Table &df3dNamespace)
{
    using namespace glm;
    using value_t = vec3::value_type;

    auto vm = svc().scripts().getVm();

    {
        Class<vec2> glmvec2Class(vm, _SC("vec2"));
        glmvec2Class
            .Ctor()
            .Ctor<value_t, value_t>()

            .Var(_SC("x"), &vec2::x)
            .Var(_SC("y"), &vec2::y)
            ;

        df3dNamespace.Bind(_SC("vec2"), glmvec2Class);
    }

    {
        Class<vec3> glmvec3Class(vm, _SC("vec3"));
        glmvec3Class
            .Ctor()
            .Ctor<value_t, value_t, value_t>()

            .Var(_SC("x"), &vec3::x)
            .Var(_SC("y"), &vec3::y)
            .Var(_SC("z"), &vec3::z)
            ;

        df3dNamespace.Bind(_SC("vec3"), glmvec3Class);
    }

    {
        Class<vec4> glmvec4Class(vm, _SC("vec4"));
        glmvec4Class
            .Ctor()
            .Ctor<value_t, value_t, value_t, value_t>()

            .Var(_SC("x"), &vec4::x)
            .Var(_SC("y"), &vec4::y)
            .Var(_SC("z"), &vec4::z)
            .Var(_SC("w"), &vec4::w)
            ;

        df3dNamespace.Bind(_SC("vec4"), glmvec4Class);
    }

    {
        Class<quat> glmquatClass(vm, _SC("quat"));
        glmquatClass
            .Ctor()
            .Ctor<value_t, value_t, value_t, value_t>()

            .Var(_SC("x"), &quat::x)
            .Var(_SC("y"), &quat::y)
            .Var(_SC("z"), &quat::z)
            .Var(_SC("w"), &quat::w)
            ;

        df3dNamespace.Bind(_SC("quat"), glmquatClass);
    }

    df3dNamespace.Func(_SC("random_int_range"), random_int_range);
    df3dNamespace.Func(_SC("random_float_range"), random_float_range);
    df3dNamespace.Func(_SC("rand"), df3d::script_impl::rand);
    df3dNamespace.Func(_SC("srand"), df3d::script_impl::srand);
    df3dNamespace.Func(_SC("srand_reset"), df3d::script_impl::srandReset);
    df3dNamespace.Func(_SC("gaussian"), MathUtils::gaussian);
    df3dNamespace.Func(_SC("slerp"), slerp);
    df3dNamespace.Func(_SC("toQuat"), toQuat);
}

void bindBase(Table &df3dNamespace)
{
    {
        Enumeration blendingMode;
        blendingMode.Const("NONE", static_cast<int>(BlendingMode::NONE));
        blendingMode.Const("ADDALPHA", static_cast<int>(BlendingMode::ADDALPHA));
        blendingMode.Const("ALPHA", static_cast<int>(BlendingMode::ALPHA));
        blendingMode.Const("ADD", static_cast<int>(BlendingMode::ADD));

        ConstTable().Enum("BlendingMode", blendingMode);
    }

    df3dNamespace.Func(_SC("executeFile"), &executeFile);
    df3dNamespace.Func(_SC("createEntity"), &createEntity);
}

void bindProcessors(Table &df3dNamespace)
{
    auto vm = svc().scripts().getVm();

    {
        Class<SceneGraphComponentProcessor, NoConstructor<SceneGraphComponentProcessor>> scGraphProcessor(vm, _SC("SceneGraphComponentProcessor"));
        scGraphProcessor
            .Func(_SC("setPosition"), &SceneGraphComponentProcessor::setPosition)
            .Func<void(SceneGraphComponentProcessor::*)(Entity, const glm::vec3 &)>(_SC("setScale"), &SceneGraphComponentProcessor::setScale)
            .Func<void(SceneGraphComponentProcessor::*)(Entity, const glm::quat &)>(_SC("setOrientation"), &SceneGraphComponentProcessor::setOrientation)

            .Func(_SC("translate"), &SceneGraphComponentProcessor::translate)
            .Func<void(SceneGraphComponentProcessor::*)(Entity, const glm::vec3 &)>(_SC("scale"), &SceneGraphComponentProcessor::scale)

            .Func(_SC("rotateYaw"), &SceneGraphComponentProcessor::rotateYaw)
            .Func(_SC("rotatePitch"), &SceneGraphComponentProcessor::rotatePitch)
            .Func(_SC("rotateRoll"), &SceneGraphComponentProcessor::rotateRoll)
            .Func(_SC("rotateAxis"), &SceneGraphComponentProcessor::rotateAxis)

            .Func(_SC("setName"), &SceneGraphComponentProcessor::setName)
            .Func(_SC("getName"), &SceneGraphComponentProcessor::getName)
            .Overload<Entity(SceneGraphComponentProcessor::*)(const std::string &)const>(_SC("getByName"), &SceneGraphComponentProcessor::getByName)
            .Overload<Entity(SceneGraphComponentProcessor::*)(Entity, const std::string &)const>(_SC("getByName"), &SceneGraphComponentProcessor::getByName)

            .Func(_SC("getWorldPosition"), &SceneGraphComponentProcessor::getWorldPosition)
            .Func(_SC("getLocalPosition"), &SceneGraphComponentProcessor::getLocalPosition)
            .Func(_SC("getLocalScale"), &SceneGraphComponentProcessor::getLocalScale)
            .Func(_SC("getWorldDirection"), &SceneGraphComponentProcessor::getWorldDirection)
            .Func(_SC("getWorldUp"), &SceneGraphComponentProcessor::getWorldUp)
            .Func(_SC("getWorldRight"), &SceneGraphComponentProcessor::getWorldRight)

            .Func(_SC("attachChild"), &SceneGraphComponentProcessor::attachChild)
            .Func(_SC("detachChild"), &SceneGraphComponentProcessor::detachChild)
            .Func(_SC("detachAllChildren"), &SceneGraphComponentProcessor::detachAllChildren)
            .Func(_SC("getParent"), &SceneGraphComponentProcessor::getParent)
            ;

        df3dNamespace.Bind(_SC("SceneGraphComponentProcessor"), scGraphProcessor);
    }

    {
        Class<Sprite2DComponentProcessor, NoConstructor<Sprite2DComponentProcessor>> sprProcessor(vm, _SC("Sprite2DComponentProcessor"));
        sprProcessor
            .Func(_SC("setAnchorPoint"), &Sprite2DComponentProcessor::setAnchorPoint)
            .Func(_SC("setZIdx"), &Sprite2DComponentProcessor::setZIdx)
            .Func(_SC("setVisible"), &Sprite2DComponentProcessor::setVisible)
            .Func(_SC("setSize"), &Sprite2DComponentProcessor::setSize)
            .Func(_SC("setWidth"), &Sprite2DComponentProcessor::setWidth)
            .Func(_SC("setHeight"), &Sprite2DComponentProcessor::setHeight)
            .Func(_SC("getSize"), &Sprite2DComponentProcessor::getSize)
            .Func(_SC("getWidth"), &Sprite2DComponentProcessor::getWidth)
            .Func(_SC("getHeight"), &Sprite2DComponentProcessor::getHeight)
            .Func(_SC("getScreenPosition"), &Sprite2DComponentProcessor::getScreenPosition)
            .Func(_SC("setBlendMode"), &Sprite2DComponentProcessor::setBlendMode2)
            .Func(_SC("setDiffuseColor"), &Sprite2DComponentProcessor::setDiffuseColor)
            .Func(_SC("setRotation"), &Sprite2DComponentProcessor::setRotation)
            .Func(_SC("add"), &Sprite2DComponentProcessor::add)
            .Func(_SC("remove"), &Sprite2DComponentProcessor::remove)
            .Func(_SC("has"), &Sprite2DComponentProcessor::has)
        ;

        df3dNamespace.Bind(_SC("Sprite2DComponentProcessor"), sprProcessor);
    }

    {
        Class<ParticleSystemComponentProcessor, NoConstructor<ParticleSystemComponentProcessor>> psProcessor(vm, _SC("ParticleSystemComponentProcessor"));
        psProcessor
            .Func(_SC("useRealStep"), &ParticleSystemComponentProcessor::useRealStep)
            .Func(_SC("useConstantStep"), &ParticleSystemComponentProcessor::useConstantStep)
            .Func(_SC("stop"), &ParticleSystemComponentProcessor::stop)
            .Func(_SC("pause"), &ParticleSystemComponentProcessor::pause)
            .Func(_SC("setSystemLifeTime"), &ParticleSystemComponentProcessor::setSystemLifeTime)
            .Func(_SC("setWorldTransformed"), &ParticleSystemComponentProcessor::setWorldTransformed)
            .Func(_SC("getSystemLifeTime"), &ParticleSystemComponentProcessor::getSystemLifeTime)
            .Func(_SC("getSystem"), &ParticleSystemComponentProcessor::getSystem)
            .Func(_SC("isWorldTransformed"), &ParticleSystemComponentProcessor::isWorldTransformed)
            .Func(_SC("isPlaying"), &ParticleSystemComponentProcessor::isPlaying)
            .Func<void(ParticleSystemComponentProcessor::*)(Entity, std::string)>(_SC("addWithResource"), &ParticleSystemComponentProcessor::addWithResource)
            .Func(_SC("remove"), &ParticleSystemComponentProcessor::remove)
            .Func(_SC("has"), &ParticleSystemComponentProcessor::has)
            ;

        df3dNamespace.Bind(_SC("ParticleSystemComponentProcessor"), psProcessor);
    }

    {
        Class<StaticMeshComponentProcessor, NoConstructor<StaticMeshComponentProcessor>> smProcessor(vm, _SC("StaticMeshComponentProcessor"));
        smProcessor
            .Func(_SC("setVisible"), &StaticMeshComponentProcessor::setVisible)
            //.Func<void(StaticMeshComponentProcessor::*)(Entity, Res)>(_SC("add"), &StaticMeshComponentProcessor::add)
            .Func(_SC("disableFrustumCulling"), &StaticMeshComponentProcessor::disableFrustumCulling)
            ;

        df3dNamespace.Bind(_SC("StaticMeshComponentProcessor"), smProcessor);
    }

    {
        Class<TagComponentProcessor, NoConstructor<TagComponentProcessor>> cls(vm, _SC("TagComponentProcessor"));
        cls
            .Func(_SC("add"), &TagComponentProcessor::add)
            ;

        df3dNamespace.Bind(_SC("TagComponentProcessor"), cls);
    }
}

void bindGame(Table &df3dNamespace)
{
    auto vm = svc().scripts().getVm();

    {
        Class<Entity> entityClass(vm, _SC("Entity"));
        entityClass
            .Ctor()
            .Prop(_SC("id"), &Entity::getID)
            .Prop(_SC("valid"), &Entity::isValid)
            ;

        df3dNamespace.Bind(_SC("Entity"), entityClass);
    }

    {
        Class<World, NoConstructor<World>> worldClass(vm, _SC("World"));
        worldClass
            .Func(_SC("alive"), &World::alive)
            .Func(_SC("destroy"), &World::destroy)
            .Func(_SC("destroyWithChildren"), &World::destroyWithChildren)
            .Func(_SC("getEntitiesCount"), &World::getEntitiesCount)
            .Func<WorldRenderingParams&(World::*)()>(_SC("getRenderingParams"), &World::getRenderingParams)

            .Prop(_SC("sceneGraph"), &World::sceneGraph)
            .Prop(_SC("tags"), &World::tags)
            .Prop(_SC("sprite2d"), &World::sprite2d)
            .Prop(_SC("vfx"), &World::vfx)
            .Prop(_SC("staticMesh"), &World::staticMesh)
            ;

        worldClass.Func(_SC("spawn"), &World::spawn);
        worldClass.Func(_SC("spawnFromFile"), &World::spawnFromFile);

        df3dNamespace.Bind(_SC("World"), worldClass);
        df3dNamespace.Func(_SC("world"), &df3dWorld);
        df3dNamespace.Func(_SC("worldCamera"), &getWorldCamera);
        df3dNamespace.Func(_SC("getScreenSize"), &getScreenSize);
    }

    {
        Class<Camera, NoConstructor<Camera>> cameraClass(vm, _SC("Camera"));
        cameraClass
            .Func(_SC("setPosition"), &Camera::setPosition)
            .Func<void(Camera::*)(const glm::quat &)>(_SC("setOrientation"), &Camera::setOrientation)

            .Func(_SC("getPosition"), &Camera::getPosition)
            .Func(_SC("getOrientation"), &Camera::getOrientation)

            .Func(_SC("getRight"), &Camera::getRight)
            .Func(_SC("getUp"), &Camera::getUp)
            .Func(_SC("getDir"), &Camera::getDir)

            .Func(_SC("worldToScreenPoint"), &Camera::worldToScreenPoint)
            ;

        df3dNamespace.Bind(_SC("Camera"), cameraClass);
    }

    {
        Class<WorldRenderingParams, NoConstructor<Light>> wrParams(vm, _SC("WorldRenderingParams"));
        wrParams
            .Func(_SC("getLightByName"), &WorldRenderingParams::getLightByName)
            ;

        df3dNamespace.Bind(_SC("WorldRenderingParams"), wrParams);
    }
}

void bindDf3d(HSQUIRRELVM vm)
{
    DefaultVM::Set(vm);

    Table df3dNamespace(vm);
    RootTable(vm).Bind("df3d", df3dNamespace);

    bindGlm(df3dNamespace);
    bindBase(df3dNamespace);
    bindGame(df3dNamespace);
    bindProcessors(df3dNamespace);
}

} }
