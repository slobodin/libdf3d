#include "stdafx.h"

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(NodeSetRotationGlm, df3d::components::TransformComponent::setOrientation, 1, 2)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(NodeGetRotationGlm, df3d::components::TransformComponent::getRotation, 0, 1)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(NodeRotateYaw, df3d::components::TransformComponent::rotateYaw, 1, 2)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(NodeRotatePitch, df3d::components::TransformComponent::rotatePitch, 1, 2)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(NodeRotateRoll, df3d::components::TransformComponent::rotateRoll, 1, 2)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(NodeRotateAxis, df3d::components::TransformComponent::rotateAxis, 2, 3)

void bindDf3dComponents()
{
    using namespace boost::python;
    using namespace df3d::components;

    enum_<ComponentType>("ComponentType")
        .value("CT_TRANSFORM", CT_TRANSFORM)
        .value("CT_MESH", CT_MESH)
        .value("CT_PARTICLE_EFFECT", CT_PARTICLE_EFFECT)
        .value("CT_AUDIO", CT_AUDIO)
        .value("CT_PHYSICS", CT_PHYSICS)
        .value("CT_LIGHT", CT_LIGHT)
        .value("CT_DEBUG_DRAW", CT_DEBUG_DRAW)
    ;

    // Base component.
    class_<NodeComponent, boost::shared_ptr<NodeComponent>, boost::noncopyable>("NodeComponent", no_init)
        .def("clone", pure_virtual(&NodeComponent::clone))
    ;

    // TransformComponent.
    void (TransformComponent::*setPositionPy)(float, float, float) = &TransformComponent::setPosition;
    void (TransformComponent::*setPositionPy1)(const glm::vec3 &) = &TransformComponent::setPosition;
    void (TransformComponent::*setScalePy)(float, float, float) = &TransformComponent::setScale;
    void (TransformComponent::*setScalePy1)(const glm::vec3 &) = &TransformComponent::setScale;
    void (TransformComponent::*setRotationPy)(const glm::quat &) = &TransformComponent::setOrientation;
    void (TransformComponent::*setRotationPy1)(const glm::vec3 &, bool) = &TransformComponent::setOrientation;
    void (TransformComponent::*translatePy)(float, float, float) = &TransformComponent::translate;
    void (TransformComponent::*translatePy1)(const glm::vec3 &) = &TransformComponent::translate;
    void (TransformComponent::*scalePy)(float, float, float) = &TransformComponent::scale;
    void (TransformComponent::*scalePy1)(const glm::vec3 &) = &TransformComponent::scale;
    class_<TransformComponent, boost::shared_ptr<TransformComponent>, boost::noncopyable, bases<NodeComponent>>("TransformComponent")
        .def("set_position", setPositionPy)
        .def("set_position", setPositionPy1)
        .def("set_scale", setScalePy)
        .def("set_scale", setScalePy1)
        .def("set_orientation", setRotationPy)
        .def("set_orientation", setRotationPy1, NodeSetRotationGlm())
        .def("translate", translatePy)
        .def("translate", translatePy1)
        .def("scale", scalePy)
        .def("scale", scalePy1)
        .def("rotate_yaw", &TransformComponent::rotateYaw, NodeRotateYaw())
        .def("rotate_pitch", &TransformComponent::rotatePitch, NodeRotatePitch())
        .def("rotate_roll", &TransformComponent::rotateRoll, NodeRotateRoll())
        .def("rotate_axis", &TransformComponent::rotateAxis, NodeRotateAxis())

        .def("get_position", &TransformComponent::getPosition)
        .def("get_scale", &TransformComponent::getScale)
        .def("get_rotation", &TransformComponent::getRotation, NodeGetRotationGlm())
        .def("get_orientation", &TransformComponent::getOrientation)
    ;

    // AudioComponent.
    class_<AudioComponent, boost::shared_ptr<AudioComponent>, boost::noncopyable, bases<NodeComponent>>("AudioComponent", init<const char *>())
        .def("play", &AudioComponent::play)
        .def("stop", &AudioComponent::stop)
        .def("pause", &AudioComponent::pause)

        .def("set_pitch", &AudioComponent::setPitch)
        .def("get_pitch", &AudioComponent::getPitch)
        .def("set_gain", &AudioComponent::setGain)
        .def("get_gain", &AudioComponent::getGain)
        .def("set_looped", &AudioComponent::setLooped)
        .def("is_looped", &AudioComponent::isLooped)

        .def("get_state", &AudioComponent::getState)
    ;

    enum_<AudioComponent::State>("AudioComponentState")
        .value("INITIAL", AudioComponent::S_INITIAL)
        .value("PLAYING", AudioComponent::S_PLAYING)
        .value("PAUSED", AudioComponent::S_PAUSED)
        .value("STOPPED", AudioComponent::S_STOPPED)
        .export_values()
    ;

    // LightComponent.
    void (LightComponent::*pySetDirection)(const glm::vec3 &) = &LightComponent::setDirection;
    void (LightComponent::*pySetDiffuse)(const glm::vec3 &) = &LightComponent::setDiffuseIntensity;
    void (LightComponent::*pySetSpecular)(const glm::vec3 &) = &LightComponent::setSpecularIntensity;
    class_<LightComponent, boost::shared_ptr<LightComponent>, boost::noncopyable, bases<NodeComponent>>("LightComponent", init<LightComponent::LightType>())
        .def("turnon", &LightComponent::turnon)
        .def("turnoff", &LightComponent::turnoff)
        .def("enabled", &LightComponent::enabled)

        .def("set_direction", pySetDirection)
        .def("set_diffuse", pySetDiffuse)
        .def("set_specular", pySetSpecular)
    ;

    enum_<LightComponent::LightType>("LightComponentType")
        .value("LIGHT_DIRECTIONAL", LightComponent::LT_DIRECTIONAL_LIGHT)
        .value("LIGHT_POINT", LightComponent::LT_POINT_LIGHT)
        .value("LIGHT_SPOT", LightComponent::LT_SPOT_LIGHT)
        .export_values()
    ;

    // MeshComponent.
    class_<MeshComponent, boost::shared_ptr<MeshComponent>, boost::noncopyable, bases<NodeComponent>>("MeshComponent", init<const char *>())
        .def("get_aabb", &MeshComponent::getAABB)
        .def("is_geometry_valid", &MeshComponent::isGeometryValid)
        .def("set_material", &MeshComponent::setMaterial)
        .def("get_material", &MeshComponent::getMaterial)
    ;

    // ParticleSystemComponent.
    class_<ParticleSystemComponent, boost::shared_ptr<ParticleSystemComponent>, boost::noncopyable, bases<NodeComponent>>("ParticleSystemComponent", init<const char *>())
        .def("stop", &ParticleSystemComponent::stop)
        .def("pause", &ParticleSystemComponent::pause)

        .def("system", &ParticleSystemComponent::getSpk, return_value_policy<reference_existing_object>())
    ;

    class_<TextMeshComponent, boost::shared_ptr<TextMeshComponent>, boost::noncopyable, bases<MeshComponent>>("TextMeshComponent", init<const char *, int>())
        .def("draw_text", &TextMeshComponent::drawText)
        .def("get_text_len", &TextMeshComponent::getTextLength)
    ;
}