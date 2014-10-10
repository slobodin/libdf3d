#include "stdafx.h"

glm::quat df3d_slerp(const glm::quat &a, const glm::quat &b, const glm::vec3::value_type &t)
{
    return glm::mix(a, b, t);
}

void bindGlm()
{
    using namespace boost::python;

    typedef glm::vec4::value_type VecType;

    // GLM bindings.
    class_<glm::vec4>("vec4")
        .def(init<VecType, VecType, VecType, VecType>())

        .def_readwrite("x", &glm::vec4::x)
        .def_readwrite("y", &glm::vec4::y)
        .def_readwrite("z", &glm::vec4::z)
        .def_readwrite("w", &glm::vec4::w)

        .def(self + glm::vec4())
        .def(self - glm::vec4())
        .def(self += glm::vec4())
        .def(self -= glm::vec4())
        .def(VecType() * self)
        .def(self * VecType())
        .def(self / VecType())
        .def(self *= VecType())
        .def(self /= VecType())
    ;

    class_<glm::vec3>("vec3")
        .def(init<VecType, VecType, VecType>())

        .def_readwrite("x", &glm::vec3::x)
        .def_readwrite("y", &glm::vec3::y)
        .def_readwrite("z", &glm::vec3::z)

        .def(self + glm::vec3())
        .def(self - glm::vec3())
        .def(self += glm::vec3())
        .def(self -= glm::vec3())
        .def(VecType() * self)
        .def(self * VecType())
        .def(self / VecType())
        .def(self *= VecType())
        .def(self /= VecType())
    ;

    class_<glm::vec2>("vec2")
        .def(init<VecType, VecType>())

        .def_readwrite("x", &glm::vec2::x)
        .def_readwrite("y", &glm::vec2::y)

        .def(self + glm::vec2())
        .def(self - glm::vec2())
        .def(self += glm::vec2())
        .def(self -= glm::vec2())
        .def(glm::vec2::value_type() * self)
        .def(self * glm::vec2::value_type())
        .def(self / glm::vec2::value_type())
        .def(self *= glm::vec2::value_type())
        .def(self /= glm::vec2::value_type())
    ;

    class_<glm::quat>("quat")
        .def(init<VecType, VecType, VecType, VecType>())
        .def(init<glm::vec3>())

        .def_readwrite("x", &glm::quat::x)
        .def_readwrite("y", &glm::quat::y)
        .def_readwrite("z", &glm::quat::z)
        .def_readwrite("w", &glm::quat::w)
    ;

    def("slerp", df3d_slerp);

    float(*glmRadians1)(const float &) = glm::radians<float>;
    glm::vec4(*glmRadians2)(const glm::vec4 &) = glm::radians<float>;
    glm::vec3(*glmRadians3)(const glm::vec3 &) = glm::radians<float>;
    glm::vec2(*glmRadians4)(const glm::vec2 &) = glm::radians<float>;
    def("radians", glmRadians1);
    def("radians", glmRadians2);
    def("radians", glmRadians3);
    def("radians", glmRadians4);

    float(*glmDegrees1)(const float &) = glm::degrees<float>;
    glm::vec4(*glmDegrees2)(const glm::vec4 &) = glm::degrees<float>;
    glm::vec3(*glmDegrees3)(const glm::vec3 &) = glm::degrees<float>;
    glm::vec2(*glmDegrees4)(const glm::vec2 &) = glm::degrees<float>;
    def("degrees", glmDegrees1);
    def("degrees", glmDegrees2);
    def("degrees", glmDegrees3);
    def("degrees", glmDegrees4);
}