#pragma once

#include <game/Entity.h>
#include <game/EntityComponentProcessor.h>

// TODO_ecs: rename to TagName

namespace df3d {

class DF3D_DLL DebugNameComponentProcessor : public EntityComponentProcessor
{
    struct Impl;
    unique_ptr<Impl> m_pimpl;

    void update() override;
    void cleanStep(const std::list<Entity> &deleted) override;

public:
    DebugNameComponentProcessor();
    ~DebugNameComponentProcessor();

    const std::string& getName(Entity e);
    Entity getByName(const std::string &name);

    void add(Entity e, const std::string &name);
    void remove(Entity e);
    bool has(Entity e);
};

}
