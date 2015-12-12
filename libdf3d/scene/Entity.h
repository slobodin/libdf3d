#pragma once

namespace df3d {

struct Entity
{
    using IdType = int32_t;

    IdType id;

    Entity(uint32_t id = -1) : id(id) { }

    bool valid() const { return id != -1; }
};

struct ComponentInstance
{
    int32_t id;

    ComponentInstance(uint32_t id = -1) : id(id) { }

    bool valid() const { return id != -1; }
};

}
