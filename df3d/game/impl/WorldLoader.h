#pragma once

namespace df3d {

class World;

namespace game_impl {

class WorldLoader
{
public:
    static void initWorld(const std::string &resourceFile, World &w);
};

} }
