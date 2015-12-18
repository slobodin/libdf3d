#include "FPSCamera.h"

#include <base/EngineController.h>
#include <input/InputEvents.h>

namespace df3d {

void FPSCamera::moveForward(float step)
{
    move(getDir() * step);
}

void FPSCamera::moveBackward(float step)
{
    move(-getDir() * step);
}

void FPSCamera::moveLeft(float step)
{
    move(-getRight() * step);
}

void FPSCamera::moveRight(float step)
{
    move(getRight() * step);
}

void FPSCamera::move(const glm::vec3 &vec)
{
    setPosition(getPosition() + vec);
}

void FPSCamera::onGameDeltaTime(float dt)
{
    if (!m_freeMove)
        return;

    float dv = dt * m_velocity;

    if (m_movingForward)
        moveForward(dv);
    if (m_movingBackward)
        moveBackward(dv);
    if (m_movingLeft)
        moveLeft(dv);
    if (m_movingRight)
        moveRight(dv);
}

void FPSCamera::onTouchEvent(const TouchEvent &touchEvent)
{
    if (!m_freeMove)
        return;

    static float yaw, pitch;

    if (touchEvent.state == TouchEvent::State::MOVING)
    {
        yaw += int(touchEvent.dx);
        pitch += int(touchEvent.dy);

        while (yaw > 360.0f)
            yaw -= 360.0f;

        while (yaw < -360.0f)
            yaw += 360.0f;

        if (pitch > 90.0f) pitch = 90.0f;
        if (pitch < -90.0f) pitch = -90.0f;

        setOrientation(glm::vec3(-pitch, -yaw, 0.0f));
    }
}

void FPSCamera::onKeyUp(const KeyboardEvent &ev)
{
    if (!m_freeMove)
        return;

    switch (ev.keycode)
    {
    case KeyboardEvent::KeyCode::KEY_UP:
        m_movingForward = false;
        break;
    case KeyboardEvent::KeyCode::KEY_DOWN:
        m_movingBackward = false;
        break;
    case KeyboardEvent::KeyCode::KEY_LEFT:
        m_movingLeft = false;
        break;
    case KeyboardEvent::KeyCode::KEY_RIGHT:
        m_movingRight = false;
        break;
    default:
        break;
    }
}

void FPSCamera::onKeyDown(const KeyboardEvent &ev)
{
    if (!m_freeMove)
        return;

    switch (ev.keycode)
    {
    case KeyboardEvent::KeyCode::KEY_UP:
        m_movingForward = true;
        break;
    case KeyboardEvent::KeyCode::KEY_DOWN:
        m_movingBackward = true;
        break;
    case KeyboardEvent::KeyCode::KEY_LEFT:
        m_movingLeft = true;
        break;
    case KeyboardEvent::KeyCode::KEY_RIGHT:
        m_movingRight = true;
        break;
    default:
        break;
    }
}

FPSCamera::FPSCamera(float velocity, bool freeMove)
    : m_freeMove(freeMove),
    m_velocity(velocity)
{
    svc().timeManager().registerTimeListener(this);
}

FPSCamera::~FPSCamera()
{
    svc().timeManager().unregisterTimeListener(this);
}

}
