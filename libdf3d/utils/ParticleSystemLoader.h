#pragma once

FWD_MODULE_CLASS(components, ParticleSystemComponent)

namespace df3d { namespace utils { namespace particle_system_loader {

void init(components::ParticleSystemComponent *component, const char *vfxDefinitionFile);

} } }