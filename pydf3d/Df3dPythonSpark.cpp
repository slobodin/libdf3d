#include "stdafx.h"

void bindSpark()
{
    using namespace boost::python;

    class_<SPK::System, boost::noncopyable, SPK::System *>("SpkSystem", no_init)
        .def("get_group", &SPK::System::getGroup, return_value_policy<reference_existing_object>())
        .def("get_nb_groups", &SPK::System::getNbGroups)
    ;

    const SPK::Particle &(SPK::Group::*getParticlePy)(size_t) const = &SPK::Group::getParticle;
    class_<SPK::Group, boost::noncopyable, SPK::Group *>("SpkGroup", no_init)
        .def("get_emitter", &SPK::Group::getEmitter, return_value_policy<reference_existing_object>())
        .def("get_nb_emitters", &SPK::Group::getNbEmitters)
        .def("get_particle", getParticlePy, return_value_policy<reference_existing_object>())
        .def("get_nb_particles", &SPK::Group::getNbParticles)
        .def("get_model", &SPK::Group::getModel, return_value_policy<reference_existing_object>())
    ;

    class_<SPK::Model, boost::noncopyable, SPK::Model *>("SpkModel", no_init)
        .def("set_life_time", &SPK::Model::setLifeTime)
    ;

    class_<SPK::Emitter, boost::noncopyable, SPK::Emitter *>("SpkEmitter", no_init)
        .def("set_active", &SPK::Emitter::setActive)
        .def("set_tank", &SPK::Emitter::setTank)
        .def("change_tank", &SPK::Emitter::changeTank)
        .def("set_flow", &SPK::Emitter::setFlow)
        .def("change_flow", &SPK::Emitter::changeFlow)
        .def("set_force", &SPK::Emitter::setForce)
        .def("is_active", &SPK::Emitter::isActive)
        .def("get_tank", &SPK::Emitter::getTank)
        .def("get_flow", &SPK::Emitter::getFlow)
        .def("get_force_min", &SPK::Emitter::getForceMin)
        .def("get_force_max", &SPK::Emitter::getForceMax)
        .def("get_zone", &SPK::Emitter::getZone, return_value_policy<reference_existing_object>())
        .def("is_full_zone", &SPK::Emitter::isFullZone)
        .def("is_sleeping", &SPK::Emitter::isSleeping)
    ;

    class_<SPK::StraightEmitter, boost::noncopyable, SPK::StraightEmitter *, bases<SPK::Emitter>>("SpkStraightEmitter", no_init)
        .def("set_direction", &SPK::StraightEmitter::setDirection)
    ;

    const SPK::Vector3D &(SPK::Particle::*getParticlePositionPy)() const = &SPK::Particle::position;
    const SPK::Vector3D &(SPK::Particle::*getParticleVelocityPy)() const = &SPK::Particle::velocity;
    class_<SPK::Particle, boost::noncopyable, SPK::Particle *>("SpkParticle", no_init)
        // FIXME:
        // Maybe we will modify these values.
        .def("position", getParticlePositionPy, return_value_policy<copy_const_reference>())
        .def("velocity", getParticleVelocityPy, return_value_policy<copy_const_reference>())

        .def("get_life_left", &SPK::Particle::getLifeLeft)
        .def("get_age", &SPK::Particle::getAge)
        .def("is_new_born", &SPK::Particle::isNewBorn)
        .def("is_alive", &SPK::Particle::isAlive)
        .def("kill", &SPK::Particle::kill)
    ;

    class_<SPK::Zone, boost::noncopyable, SPK::Zone *>("SpkZone", no_init)
        .def("set_position", &SPK::Zone::setPosition)
        .def("get_position", &SPK::Zone::getPosition, return_value_policy<copy_const_reference>())
    ;

    class_<SPK::Vector3D>("SpkVec3d")
        .def(init<float, float, float>())

        .def_readwrite("x", &SPK::Vector3D::x)
        .def_readwrite("y", &SPK::Vector3D::y)
        .def_readwrite("z", &SPK::Vector3D::z)
    ;
}