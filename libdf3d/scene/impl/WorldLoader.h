#pragma once

namespace df3d {

class World;

namespace scene_impl {

class WorldLoader
{
public:
    static unique_ptr<World> createWorld(const std::string &resourceFile);
};

} }
