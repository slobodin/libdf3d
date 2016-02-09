#include "ScriptBindings.h"

#include <sqrat/sqrat.h>

#include <libdf3d/utils/Utils.h>
#include <libdf3d/utils/MathUtils.h>
#include <libdf3d/base/EngineController.h>
#include <libdf3d/game/Entity.h>
#include <libdf3d/game/World.h>

using namespace Sqrat;

namespace df3d { namespace script_impl {

inline float random_float()
{
    return utils::RandRange(0.0f, 1.0f);
}

inline float random_float_range(float a, float b)
{
    return utils::RandRange(a, b);
}

inline int random_int_range(int a, int b)
{
    return utils::RandRange(a, b);
}

inline World* df3dWorld()
{
    return &df3d::world();
}

void bindGlm(Table &df3dNamespace)
{
    using namespace glm;
    using value_t = vec3::value_type;

    auto vm = df3d::svc().scripts().getVm();

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

    df3dNamespace.Func(_SC("random_float"), random_float);
    df3dNamespace.Func(_SC("random_float_range"), random_float_range);
    df3dNamespace.Func(_SC("random_int_range"), random_int_range);
    df3dNamespace.Func(_SC("gaussian"), df3d::utils::math::gaussian);
}

void bindProcessors(Table &df3dNamespace)
{
    auto vm = df3d::svc().scripts().getVm();

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

            .Func(_SC("getWorldPosition"), &SceneGraphComponentProcessor::getWorldPosition)
            .Func(_SC("getLocalPosition"), &SceneGraphComponentProcessor::getLocalPosition)
            .Func(_SC("getScale"), &SceneGraphComponentProcessor::getScale)
            .Func(_SC("getOrientation"), &SceneGraphComponentProcessor::getOrientation)
            .Func(_SC("getRotation"), &SceneGraphComponentProcessor::getRotation)
            .Func(_SC("getWorldDirection"), &SceneGraphComponentProcessor::getWorldDirection)
            ;

        df3dNamespace.Bind(_SC("SceneGraphComponentProcessor"), scGraphProcessor);
    }
}

void bindGame(Table &df3dNamespace)
{
    auto vm = df3d::svc().scripts().getVm();

    {
        Class<Entity> entityClass(vm, _SC("Entity"));
        entityClass
            .Ctor()
            .Prop(_SC("valid"), &Entity::valid)
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

            .Prop(_SC("sceneGraph"), &World::sceneGraph)
            ;

        worldClass.Overload<Entity(World::*)()>(_SC("spawn"), &World::spawn);
        worldClass.Overload<Entity(World::*)(const std::string &)>(_SC("spawn"), &World::spawn);

        df3dNamespace.Bind(_SC("World"), worldClass);
        df3dNamespace.Func(_SC("world"), &df3dWorld);
    }
}

void bindDf3d(HSQUIRRELVM vm)
{
    DefaultVM::Set(vm);

    Table df3dNamespace(vm);
    RootTable(vm).Bind("df3d", df3dNamespace);

    bindGlm(df3dNamespace);
    bindGame(df3dNamespace);
    bindProcessors(df3dNamespace);
}

} }
