#pragma once

#include <3d/Camera.h>
#include <base/TimeManager.h>

namespace df3d {

class DF3D_DLL FPSCamera : public Camera
{
    bool m_freeMove;
    float m_velocity = 0.0f;
    float m_damping = 0.5f;

    void move(const glm::vec3 &vec);
    void onUpdate();

    TimeManager::Handle m_updateHandle;

public:
    FPSCamera(float velocity, bool freeMove = true, float damping = 0.5f);
    ~FPSCamera();

    void setFreeMove(bool freeMove) { m_freeMove = freeMove; }
    bool getFreeMove() const { return m_freeMove; }
};

}
