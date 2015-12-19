#include "df3d_pch.h"
#include "WorldLoader.h"

#include <scene/World.h>

namespace df3d { namespace scene_impl {

unique_ptr<World> WorldLoader::createWorld(const std::string &resourceFile)
{
    auto res = World::newWorld();

    return res;
}

} }