#include "df3d_pch.h"
#include "df3d_bindings.h"

#include <luabind/luabind.hpp>
#include <luabind/operator.hpp>

namespace df3d { namespace scripting {

void df3dLog(const char *msg)
{
    base::glog << msg << base::loglua;
}

void bindGlm(lua_State *L)
{
    using namespace luabind;
    using value_type = glm::vec3::value_type;

    module(L, "glm")
    [
        class_<glm::vec4>("vec4")
            .def(constructor<>())
            .def(constructor<value_type, value_type, value_type, value_type>())

            .def_readonly("x", &glm::vec4::x)
            .def_readonly("y", &glm::vec4::y)
            .def_readonly("z", &glm::vec4::z)
            .def_readonly("w", &glm::vec4::w)

            .def(self + glm::vec4())
            .def(self - glm::vec4())
            .def(value_type() * self)
            .def(self * value_type())
            .def(self / value_type())
        ,

        class_<glm::vec3>("vec3")
            .def(constructor<>())
            .def(constructor<value_type, value_type, value_type>())

            .def_readonly("x", &glm::vec3::x)
            .def_readonly("y", &glm::vec3::y)
            .def_readonly("z", &glm::vec3::z)

            .def(self + glm::vec3())
            .def(self - glm::vec3())
            .def(value_type() * self)
            .def(self * value_type())
            .def(self / value_type())
        ,

        class_<glm::vec2>("vec2")
            .def(constructor<>())
            .def(constructor<value_type, value_type>())

            .def_readonly("x", &glm::vec2::x)
            .def_readonly("y", &glm::vec2::y)

            .def(self + glm::vec2())
            .def(self - glm::vec2())
            .def(value_type() * self)
            .def(self * value_type())
            .def(self / value_type())
    ];
}

} }
