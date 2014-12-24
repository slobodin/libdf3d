#pragma once

FWD_MODULE_CLASS(components, LightComponent)

namespace df3d { namespace components { namespace serializers {

DF3D_DLL void load(components::LightComponent *component, const Json::Value &root);
DF3D_DLL Json::Value save(const components::LightComponent *component);

} } }