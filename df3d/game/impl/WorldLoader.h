#pragma once

namespace df3d {

class World;

namespace game_impl {

class WorldLoader
{
public:
    static void initWorld(const char *resourceFile, World &w);
};

} }
