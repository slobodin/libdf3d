#include "df3d_pch.h"
#include "FPSCamera.h"

#include <base/InputEvents.h>

namespace df3d { namespace scene {

FPSCamera::FPSCamera(float velocity, bool freeMove)
    : m_freeMove(freeMove),
    m_velocity(velocity)
{
}

FPSCamera::~FPSCamera()
{
}

void FPSCamera::onUpdate(float dt)
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

void FPSCamera::onMouseMotionEvent(const base::MouseMotionEvent &mouseMotionEvent)
{
    if (!m_freeMove)
        return;

    static int yaw, pitch;
    static int prevx, prevy;

    if (mouseMotionEvent.leftPressed)
    {
        int dx = mouseMotionEvent.x - prevx;
        int dy = mouseMotionEvent.y - prevy;

        yaw += dx;
        pitch += dy;

        if (abs(yaw) > 360) yaw %= 360;
        if (pitch > 90) pitch = 90;
        if (pitch < -90) pitch = -90;

        setRotation((float)-yaw, (float)-pitch, 0);
    }

    prevx = mouseMotionEvent.x;
    prevy = mouseMotionEvent.y;
}

void FPSCamera::onKeyUp(const SDL_KeyboardEvent &keyEvent)
{
    if (!m_freeMove)
        return;

    switch (keyEvent.keysym.sym)
    {
    case SDLK_UP:
        m_movingForward = false;
        break;
    case SDLK_DOWN:
        m_movingBackward = false;
        break;
    case SDLK_LEFT:
        m_movingLeft = false;
        break;
    case SDLK_RIGHT:
        m_movingRight = false;
        break;
    default:
        break;
    }
}

void FPSCamera::onKeyDown(const SDL_KeyboardEvent &keyEvent)
{
    if (!m_freeMove)
        return;

    switch (keyEvent.keysym.sym)
    {
    case SDLK_UP:
        m_movingForward = true;
        break;
    case SDLK_DOWN:
        m_movingBackward = true;
        break;
    case SDLK_LEFT:
        m_movingLeft = true;
        break;
    case SDLK_RIGHT:
        m_movingRight = true;
        break;
    default:
        break;
    }
}

} }