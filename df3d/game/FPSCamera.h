#pragma once

#include <df3d/engine/3d/Camera.h>
#include <df3d/engine/TimeManager.h>

namespace df3d {

class DF3D_DLL FPSCamera : public Camera
{
    bool m_freeMove;
    float m_velocity = 0.0f;
    float m_damping = 0.5f;

    void move(const glm::vec3 &vec);
    void onUpdate();

    TimeManagerHandle m_updateHandle;

public:
    FPSCamera(float velocity, bool freeMove, float damping, const glm::vec3 &position, float fov, float nearZ, float farZ);
    ~FPSCamera();

    void setFreeMove(bool freeMove) { m_freeMove = freeMove; }
    bool getFreeMove() const { return m_freeMove; }

    static shared_ptr<FPSCamera> createDefault(float velocity)
    {
        return make_shared<FPSCamera>(velocity, true, 0.5f, glm::vec3(), 60.0f, 0.1f, 5000.0f);
    }
};

}
