#pragma once

#include <df3d/game/Entity.h>
#include <df3d/game/EntityComponentProcessor.h>

namespace df3d {

class TagComponentProcessor : public EntityComponentProcessor
{
    // Tag to entities with this tag lookup.
    std::unordered_map<Id, std::unordered_set<Entity>> m_entities;
    // Entity to tag list lookup.
    std::unordered_map<Entity, std::unordered_set<Id>> m_tagLookup;

    void update() override { }

public:
    TagComponentProcessor() = default;
    ~TagComponentProcessor() = default;

    const std::unordered_set<Entity>& getEntities(Id tag);
    int getCountByTag(Id tag);
    const std::unordered_set<Id>* getTags(Entity e);
    Entity getFirst(Id tag);
    bool hasTag(Entity e, Id tag) const;

    void removeTag(Entity e, Id tag);

    void add(Entity e, Id tag);
    void remove(Entity e) override;
    bool has(Entity e) override;
};

}
