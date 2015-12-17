#pragma once

#include <scene/Camera.h>
#include <base/TimeManager.h>
#include <input/InputManager.h>

namespace df3d {

class DF3D_DLL FPSCamera : public Camera, public TimeListener, public InputListener
{
    bool m_movingForward = false, m_movingBackward = false, m_movingLeft = false, m_movingRight = false;
    bool m_freeMove;
    float m_velocity = 0.0f;

    void moveForward(float step);
    void moveBackward(float step);
    void moveLeft(float step);
    void moveRight(float step);
    void move(const glm::vec3 &vec);

    void onGameDeltaTime(float dt) override;
    void onTouchEvent(const TouchEvent &touchEvent) override;
    void onKeyUp(const KeyboardEvent &code) override;
    void onKeyDown(const KeyboardEvent &code) override;

public:
    FPSCamera(float velocity, bool freeMove = true);
    ~FPSCamera();

    void setFreeMove(bool freeMove) { m_freeMove = freeMove; }
    bool getFreeMove() const { return m_freeMove; }
};

}
