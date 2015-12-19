#pragma once

namespace df3d {

DF3D_DLL SceneNode newNode(const std::string &name = "");
DF3D_DLL SceneNode nodeFromFile(const std::string &jsonDefinitionFile);
DF3D_DLL SceneNode nodeFromJson(const Json::Value &root);
DF3D_DLL Json::Value saveNode(SceneNode node);

}
