#pragma once

#include "NodeComponent.h"

namespace df3d { namespace components {

//! Creates node component from a file with json description.
DF3D_DLL shared_ptr<NodeComponent> create(ComponentType type, const std::string &jsonFile);
//! Creates node component of given type from JSON value.
DF3D_DLL shared_ptr<NodeComponent> create(ComponentType type, const Json::Value &root);
//! Serializes given component to a JSON value.
DF3D_DLL Json::Value toJson(shared_ptr<const NodeComponent> component);

} }
