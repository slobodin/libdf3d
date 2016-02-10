#include "ScriptBindings.h"

#include <sqrat/sqrat.h>

#include <libdf3d/utils/Utils.h>
#include <libdf3d/utils/MathUtils.h>
#include <libdf3d/base/EngineController.h>
#include <libdf3d/game/Entity.h>
#include <libdf3d/game/World.h>
#include <libdf3d/2d/Sprite2DComponentProcessor.h>

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

inline Camera* getWorldCamera()
{
    return df3d::world().getCamera().get();
}

inline glm::vec2 getScreenSize()
{
    return df3d::svc().getScreenSize();
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

    df3dNamespace.Func(_SC("random_float"), random_float);
    df3dNamespace.Func(_SC("random_float_range"), random_float_range);
    df3dNamespace.Func(_SC("random_int_range"), random_int_range);
    df3dNamespace.Func(_SC("gaussian"), df3d::utils::math::gaussian);
}

void bindBase(Table &df3dNamespace)
{
    {
        Enumeration blendingMode;
        blendingMode.Const("NONE", static_cast<int>(RenderPass::BlendingMode::NONE));
        blendingMode.Const("ADDALPHA", static_cast<int>(RenderPass::BlendingMode::ADDALPHA));
        blendingMode.Const("ALPHA", static_cast<int>(RenderPass::BlendingMode::ALPHA));
        blendingMode.Const("ADD", static_cast<int>(RenderPass::BlendingMode::ADD));

        ConstTable().Enum("BlendingMode", blendingMode);
    }
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
            .Overload<Entity(SceneGraphComponentProcessor::*)(const std::string &)const>(_SC("getByName"), &SceneGraphComponentProcessor::getByName)
            .Overload<Entity(SceneGraphComponentProcessor::*)(Entity, const std::string &)const>(_SC("getByName"), &SceneGraphComponentProcessor::getByName)

            .Func(_SC("getWorldPosition"), &SceneGraphComponentProcessor::getWorldPosition)
            .Func(_SC("getLocalPosition"), &SceneGraphComponentProcessor::getLocalPosition)
            .Func(_SC("getScale"), &SceneGraphComponentProcessor::getScale)
            .Func(_SC("getOrientation"), &SceneGraphComponentProcessor::getOrientation)
            .Func(_SC("getRotation"), &SceneGraphComponentProcessor::getRotation)
            .Func(_SC("getWorldDirection"), &SceneGraphComponentProcessor::getWorldDirection)

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
            .Prop(_SC("sprite2d"), &World::sprite2d)
            ;

        worldClass.Overload<Entity(World::*)()>(_SC("spawn"), &World::spawn);
        worldClass.Overload<Entity(World::*)(const std::string &)>(_SC("spawn"), &World::spawn);

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
            ;

        df3dNamespace.Bind(_SC("Camera"), cameraClass);
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
