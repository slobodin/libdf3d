#pragma once

#include <components/NodeComponent.h>

namespace df3d {

//! Creates node component from a file with json description.
DF3D_DLL Component componentFromFile(components::ComponentType type, const std::string &jsonFile);
//! Creates node component of given type from JSON value.
DF3D_DLL Component componentFromJson(components::ComponentType type, const Json::Value &root);
//! Serializes given component to a JSON value.
DF3D_DLL Json::Value saveComponent(Component component);

}
