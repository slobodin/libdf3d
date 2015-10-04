#pragma once

namespace df3d { namespace scene {

DF3D_DLL shared_ptr<Node> createNode(const std::string &jsonDefinitionFile);
DF3D_DLL shared_ptr<Node> createNode(const Json::Value &root);
DF3D_DLL Json::Value nodeToJson(shared_ptr<const Node> node);

} }
