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

    {
        Class<vec2> glmvec2Class;
        glmvec2Class
            .Ctor()
            .Ctor<value_t, value_t>()

            .Var(_SC("x"), &vec2::x)
            .Var(_SC("y"), &vec2::y)
            ;

        df3dNamespace.Bind(_SC("vec2"), glmvec2Class);
    }

    {
        Class<vec3> glmvec3Class;
        glmvec3Class
            .Ctor()
            .Ctor<value_t, value_t, value_t>()

            .Var(_SC("x"), &vec3::x)
            .Var(_SC("y"), &vec3::y)
            .Var(_SC("z"), &vec3::z)
            ;

        df3dNamespace.Bind(_SC("vec3"), glmvec3Class);
    }

    df3dNamespace.Func(_SC("random_float"), random_float);
    df3dNamespace.Func(_SC("random_float_range"), random_float_range);
    df3dNamespace.Func(_SC("random_int_range"), random_int_range);
    df3dNamespace.Func(_SC("gaussian"), df3d::utils::math::gaussian);
}

void bindProcessors()
{

}

void bindGame(Table &df3dNamespace)
{
    {
        Class<Entity> entityClass;
        entityClass
            .Ctor()
            .Prop(_SC("valid"), &Entity::valid)
        ;

        df3dNamespace.Bind(_SC("Entity"), entityClass);
    }

    {
        Class<World, NoConstructor<World>> worldClass;
        worldClass
            .Func(_SC("alive"), &World::alive)
            .Func(_SC("destroy"), &World::destroy)
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
}

} }
