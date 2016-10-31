#pragma once

#include <df3d/game/Entity.h>
#include <df3d/game/EntityComponentProcessor.h>

namespace df3d {

class TagComponentProcessor : public EntityComponentProcessor
{
    // Tag to entities with this tag lookup.
    std::unordered_map<int, std::unordered_set<Entity>> m_entities;
    // Entity to tag list lookup.
    std::unordered_map<Entity, std::unordered_set<int>> m_tagLookup;

    void update() override { }

public:
    TagComponentProcessor() = default;
    ~TagComponentProcessor() = default;

    const std::unordered_set<Entity>& getEntities(int tag);
    const std::unordered_set<int>* getTags(Entity e);
    Entity getFirst(int tag);
    bool hasTag(Entity e, int tag) const;

    void add(Entity e, int tag);
    void remove(Entity e) override;
    bool has(Entity e) override;
};

}
