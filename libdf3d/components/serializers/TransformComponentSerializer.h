#pragma once

FWD_MODULE_CLASS(components, TransformComponent)

namespace df3d { namespace components { namespace serializers {

DF3D_DLL void load(components::TransformComponent *component, const Json::Value &root);
DF3D_DLL Json::Value save(components::TransformComponent *component);

} } }