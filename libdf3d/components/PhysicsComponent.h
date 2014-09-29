#pragma once

#include "NodeComponent.h"

class btRigidBody;

namespace df3d { namespace components {

class DF3D_DLL PhysicsComponent : public NodeComponent
{
    std::string m_definitionFile;
    short m_group = -1;
    short m_mask = -1;

    //void onUpdate(float dt);
    void onAttached();

public:
    btRigidBody *body = nullptr;

    // Holder needs to have a valid (loaded) mesh.
    // TODO:
    // Patch for async, do not demand valid mesh.
    PhysicsComponent(const char *definitionFile, short group = -1, short mask = -1);
    ~PhysicsComponent();

    shared_ptr<NodeComponent> clone() const;
};

} }