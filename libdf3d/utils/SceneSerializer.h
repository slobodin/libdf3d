#pragma once

FWD_MODULE_CLASS(scene, Scene)

namespace df3d { namespace utils { namespace serializers {

DF3D_DLL void load(scene::Scene *sc, const char *sceneDefinitionFile);
DF3D_DLL void save(const scene::Scene *sc, const char *sceneDefinitionFile);

} } }