#pragma once

namespace df3d {

struct Entity
{
    using IdType = int32_t;

    IdType id;

    Entity(IdType id = -1) : id(id) { }

    bool valid() const { return id != -1; }
};

struct ComponentInstance
{
    using IdType = int32_t;

    IdType id;

    ComponentInstance(IdType id = -1) : id(id) { }

    bool valid() const { return id != -1; }
};

}
