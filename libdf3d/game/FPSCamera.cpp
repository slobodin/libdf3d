#include "FPSCamera.h"

#include <libdf3d/base/EngineController.h>
#include <libdf3d/game/World.h>
#include <libdf3d/input/InputManager.h>
#include <libdf3d/input/InputEvents.h>

namespace df3d {

void FPSCamera::move(const glm::vec3 &vec)
{
    setPosition(getPosition() + vec);
}

void FPSCamera::onUpdate()
{
    if (!m_freeMove)
        return;

    if (svc().inputManager().getMouseButton(MouseButton::LEFT))
    {
        static float yaw, pitch;
        yaw += int(svc().inputManager().getMouseDelta().x) * m_damping;
        pitch += int(svc().inputManager().getMouseDelta().y) * m_damping;

        while (yaw > 360.0f)
            yaw -= 360.0f;

        while (yaw < -360.0f)
            yaw += 360.0f;

        if (pitch > 90.0f) pitch = 90.0f;
        if (pitch < -90.0f) pitch = -90.0f;

        setOrientation(glm::vec3(-pitch, -yaw, 0.0f));
    }

    float dt = svc().timer().getFrameDelta(TimeChannel::GAME);
    float dv = dt * m_velocity;

    if (svc().inputManager().getKey(KeyCode::KEY_UP))
        move(getDir() * dv);
    if (svc().inputManager().getKey(KeyCode::KEY_DOWN))
        move(-getDir() * dv);
    if (svc().inputManager().getKey(KeyCode::KEY_LEFT))
        move(-getRight() * dv);
    if (svc().inputManager().getKey(KeyCode::KEY_RIGHT))
        move(getRight() * dv);
}

FPSCamera::FPSCamera(float velocity, bool freeMove, float damping, const glm::vec3 &position, float fov, float nearZ, float farZ)
    : Camera(position, fov, nearZ, farZ),
    m_freeMove(freeMove),
    m_velocity(velocity),
    m_damping(damping)
{
    m_updateHandle = svc().world().timeManager().subscribeUpdate(std::bind(&FPSCamera::onUpdate, this));
}

FPSCamera::~FPSCamera()
{
    svc().world().timeManager().unsubscribeUpdate(m_updateHandle);
}

}
