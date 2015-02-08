#include "df3d_pch.h"
#include "df3d_bindings.h"

#include <lua/lua.hpp>
#include <LuaBridge/LuaBridge.h>

namespace df3d { namespace scripting {

float vecDot(const glm::vec3 &a, const glm::vec3 &b)
{
    return glm::dot(a, b);
}

void bindGlm(lua_State *L)
{
    using namespace luabridge;

    getGlobalNamespace(L)
        .beginNamespace("glm")
            .beginClass <glm::vec3>("vec3")
                .addConstructor<void(*)()>()
                .addData("x", &glm::vec3::x)
                .addData("y", &glm::vec3::y)
                .addData("z", &glm::vec3::z)
            .endClass()

            .addFunction("dot", vecDot)

        .endNamespace();
}

} }
