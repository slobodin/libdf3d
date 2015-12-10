#pragma once

namespace df3d {

struct Entity
{
    uint32_t id = 0;
};

struct ComponentInstance
{
    uint32_t id = 0;

    bool valid() const { return id != 0; }
};

}
