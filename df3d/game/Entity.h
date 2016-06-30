#pragma once

namespace df3d {

struct Entity
{
    using IdType = int32_t;

    IdType id;

    Entity(IdType id = -1) : id(id) { }

    IdType getId() const { return id; }     // Squirrel workaround.
    bool valid() const { return id != -1; }
    bool operator== (const Entity &other) const { return other.id == id; }
    bool operator!= (const Entity &other) const { return other.id != id; }
};

struct ComponentInstance
{
    using IdType = int32_t;

    IdType id;

    ComponentInstance(IdType id = -1) : id(id) { }

    bool valid() const { return id != -1; }
};

}

namespace std {

template <>
struct hash<df3d::Entity>
{
    std::size_t operator()(const df3d::Entity& e) const
    {
        return std::hash<df3d::Entity::IdType>()(e.id);
    }
};

}
