#include "stdafx.h"

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(CameraSetRotation, df3d::scene::Camera::setRotation, 3, 4)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(CameraGetRotation, df3d::scene::Camera::getRotation, 0, 1)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(CameraScreenToView, df3d::scene::Camera::screenToViewPoint, 2, 3)

void bindDf3dScene()
{
    using namespace boost::python;
    using namespace df3d::scene;

    // Scene manager.
    void (SceneManager::*removeNodeFromCurrentScenePy)(boost::shared_ptr<Node>) = &SceneManager::removeNodeFromScene;
    void (SceneManager::*removeNodeFromCurrentScenePyStr)(const char *) = &SceneManager::removeNodeFromScene;
    class_<SceneManager, boost::noncopyable>("SceneManager", no_init)
        .def("get_camera", &SceneManager::getCamera)
        .def("set_camera", &SceneManager::setCamera)

        .def("set_current_scene", &SceneManager::setCurrentScene)
        .def("get_current_scene", &SceneManager::getCurrentScene)

        .def("add_node_to_scene", &SceneManager::addNodeToScene)
        .def("remove_node_from_scene", removeNodeFromCurrentScenePy)
        .def("remove_node_from_scene", removeNodeFromCurrentScenePyStr)
        
        .def("clear_current_scene", &SceneManager::clearCurrentScene)
        .def("pause_simulation", &SceneManager::pauseSimulation)
    ;

    // Camera.
    class_<Camera, boost::shared_ptr<Camera>, boost::noncopyable>("Camera",
        init<optional<glm::vec3, float, float, float>>())
        .def("move_forward", &Camera::moveForward)
        .def("move_backward", &Camera::moveBackward)
        .def("move_left", &Camera::moveLeft)
        .def("move_right", &Camera::moveRight)
        .def("move", &Camera::move)
        .def("look_at", &Camera::lookAt)
        .def("set_fov", &Camera::setFov)

        .def("get_rotation", &Camera::getRotation, CameraGetRotation())
        .def("set_rotation", &Camera::setRotation, CameraSetRotation())

        .def("get_position", &Camera::getPosition)
        .def("set_position", &Camera::setPosition)

        .def("get_right", &Camera::getRight, return_value_policy<copy_const_reference>())
        .def("get_up", &Camera::getUp, return_value_policy<copy_const_reference>())
        .def("get_dir", &Camera::getDir, return_value_policy<copy_const_reference>())

        .def("get_fov", &Camera::getFov)
        .def("get_nearz", &Camera::getNearZ)
        .def("get_farz", &Camera::getFarZ)

        .def("screen_to_view", &Camera::screenToViewPoint, CameraScreenToView())
        .def("world_to_screen", &Camera::worldToScreenPoint)
    ;

    // AABB.
    class_<AABB>("AABB")
        .def("is_valid", &AABB::isValid)
        .def("contains", &AABB::contains)
        //.def("intersects", &AABB::intersects)

        .def("get_center", &AABB::getCenter)
        .def("get_min", &AABB::minPoint, return_value_policy<copy_const_reference>())
        .def("get_max", &AABB::maxPoint, return_value_policy<copy_const_reference>())

        .def("reset", &AABB::reset)
    ;

    // TODO:
    // Other bounding volumes.

    // Scene.
    void (Scene::*setFogPy)(float, float, float, float) = &Scene::setFog;
    void (Scene::*setFogPy1)(float density, const glm::vec3 &) = &Scene::setFog;
    class_<Scene, boost::shared_ptr<Scene>, boost::noncopyable>("Scene", no_init)
        .def("set_fog", setFogPy)
        .def("set_fog", setFogPy1)
        .def("get_fog_color", &Scene::getFogColor)
        .def("get_fog_density", &Scene::getFogDensity)

        .def("set_ambient_light", &Scene::setAmbientLight)
        .def("get_amibient_light", &Scene::getAmbientLight)

        // add_childs.

        .def("get_root", &Scene::getRoot)
        .def("get_child_by_name", &Scene::getChildByName)
    ;

    // In order to iterate over node children.
    class_<std::map<std::string, boost::shared_ptr<Node>> >("NodeMap")
        .def(map_indexing_suite<std::map<std::string, boost::shared_ptr<Node>>, true>());

    // Node.
    void (Node::*removeChildPy)(boost::shared_ptr<Node>) = &Node::removeChild;
    void (Node::*removeChildPy1)(const char *) = &Node::removeChild;
    class_<Node, boost::shared_ptr<Node>, boost::noncopyable>("Node")
        .def(init<optional<const char *>>())
        .def("set_name", &Node::setName)
        .def("get_name", &Node::getName, return_value_policy<copy_const_reference>())

        .def("set_visible", &Node::setVisible)
        .def("is_visible", &Node::isVisible)

        .def("add_child", &Node::addChild)
        .def("remove_child", removeChildPy)
        .def("remove_child", removeChildPy1)
        .def("remove_all_children", &Node::removeAllChildren)

        .def("get_child_by_name", &Node::getChildByName)
        .def("get_parent", &Node::getParent)

        .def("clone", &Node::clone)
        .def("__iter__", range(&Node::begin, &Node::end))

        .add_property("transform", &Node::transform)
        .add_property("mesh", &Node::mesh)
        .add_property("light", &Node::light)
        .add_property("audio", &Node::audio)
        .add_property("vfx", &Node::vfx)
        .add_property("physics", &Node::physics)

        .def("attach_component", &Node::attachComponent)
        .def("detach_component", &Node::detachComponent)

        .def("from_file", &Node::fromFile)
        .staticmethod("from_file")
    ;
}