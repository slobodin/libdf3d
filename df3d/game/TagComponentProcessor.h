#pragma once

#include <df3d/game/Entity.h>
#include <df3d/game/EntityComponentProcessor.h>

namespace df3d {

class DF3D_DLL TagComponentProcessor : public EntityComponentProcessor
{
    std::unordered_map<int, std::unordered_set<Entity>> m_entities;
    std::unordered_map<Entity, std::unordered_set<int>> m_tagLookup;

    void update() override;
    void cleanStep(const std::list<Entity> &deleted) override;

public:
    TagComponentProcessor();
    ~TagComponentProcessor();

    const std::unordered_set<Entity>& getEntities(int tag);
    const std::unordered_set<int>* getTags(Entity e);
    Entity getFirst(int tag);
    bool hasTag(Entity e, int tag) const;

    // NOTE: can have only 1 tag for now.
    void add(Entity e, int tag);
    void remove(Entity e);
};

}
