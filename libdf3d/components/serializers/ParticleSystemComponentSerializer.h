#pragma once

FWD_MODULE_CLASS(components, ParticleSystemComponent)

namespace df3d { namespace components { namespace serializers {

DF3D_DLL void load(components::ParticleSystemComponent *component, const char *vfxDefinitionFile);
DF3D_DLL void save(const components::ParticleSystemComponent *component, const char *vfxDefinitionFile);

} } }