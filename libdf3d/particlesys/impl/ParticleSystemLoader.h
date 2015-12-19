#pragma once

#include <SPARK.h>

namespace df3d { namespace particlesys_impl {

class ParticleSystemLoader
{
public:
    SPK::Ref<SPK::System> createSpkSystem(const Json::Value &root);
    SPK::Ref<SPK::System> createSpkSystem(const std::string &vfxFile);
};

} }
