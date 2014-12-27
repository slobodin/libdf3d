#pragma once

FWD_MODULE_CLASS(scene, Scene)

namespace df3d { namespace utils { namespace serializers {

DF3D_DLL shared_ptr<scene::Scene> fromJson(const Json::Value &root);
DF3D_DLL Json::Value toJson(shared_ptr<const scene::Scene> scene);

} } }