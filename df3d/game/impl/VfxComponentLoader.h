#pragma once

#include <df3d/game/EntityComponentLoader.h>
#include <df3d/engine/particlesys/ParticleSystemComponentProcessor.h>
#include <df3d/engine/particlesys/ParticleSystemUtils.h>

namespace df3d {

class VfxComponentLoader : public EntityComponentLoader
{
    mutable std::unordered_map<std::string, ParticleSystemCreationParams> m_cache;

public:
    void loadComponent(const Json::Value &root, Entity e, World &w) const override
    {
        auto pathStr = root["path"].asString();
        auto found = m_cache.find(pathStr);
        if (found == m_cache.end())
            m_cache[pathStr] = ParticleSystemUtils::parseVfx(pathStr.c_str());

        w.vfx().add(e, m_cache[pathStr].clone());
    }
};

}
