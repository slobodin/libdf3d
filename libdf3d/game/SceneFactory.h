#pragma once

FWD_MODULE_CLASS(scene, Scene)

namespace df3d {

DF3D_DLL shared_ptr<scene::Scene> newScene();
DF3D_DLL shared_ptr<scene::Scene> sceneFromFile(const std::string &jsonFile);
DF3D_DLL shared_ptr<scene::Scene> sceneFromJson(const Json::Value &root);
DF3D_DLL Json::Value saveScene(shared_ptr<const scene::Scene> scene);

}