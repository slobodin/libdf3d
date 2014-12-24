#pragma once

FWD_MODULE_CLASS(components, AudioComponent)

namespace df3d { namespace components { namespace serializers {

DF3D_DLL Json::Value save(const components::AudioComponent *component);

} } }