#include "ScriptBindings.h"

#include <utils/Utils.h>
#include <utils/MathUtils.h>
#include <base/EngineController.h>
#include <game/Entity.h>
#include <game/World.h>

using namespace Sqrat;

namespace df3d { namespace script_impl {

inline glm::vec3 vec3_sub(const glm::vec3 &a, const glm::vec3 &b)
{
    return a - b;
}

inline glm::vec3 vec3_add(const glm::vec3 &a, const glm::vec3 &b)
{
    return a + b;
}

inline glm::vec3 vec3_mult_scalar(const glm::vec3 &a, float s)
{
    return a * s;
}

inline float vec3_dot(const glm::vec3 &a, const glm::vec3 &b)
{
    return glm::dot(a, b);
}

inline glm::vec3 vec3_cross(const glm::vec3 &a, const glm::vec3 &b)
{
    return glm::cross(a, b);
}

inline float vec3_len(const glm::vec3 &a)
{
    return glm::length(a);
}

inline glm::vec3 vec3_neg(const glm::vec3 &a)
{
    return -a;
}

inline float random_float()
{
    return utils::RandRange(0.0f, 1.0f);
}

inline float random_range(float a, float b)
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

    df3dNamespace.Func(_SC("safeNormalize"), df3d::utils::math::safeNormalize);
    df3dNamespace.Func(_SC("vec3_sub"), vec3_sub);
    df3dNamespace.Func(_SC("vec3_add"), vec3_add);
    df3dNamespace.Func(_SC("vec3_dot"), vec3_dot);
    df3dNamespace.Func(_SC("vec3_mult_scalar"), vec3_mult_scalar);
    df3dNamespace.Func(_SC("vec3_cross"), vec3_cross);
    df3dNamespace.Func(_SC("vec3_len"), vec3_len);
    df3dNamespace.Func(_SC("vec3_neg"), vec3_neg);
    df3dNamespace.Func(_SC("random_float"), random_float);
    df3dNamespace.Func(_SC("random_range"), random_range);
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
