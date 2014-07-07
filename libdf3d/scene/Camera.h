#pragma once

FWD_MODULE_CLASS(render, Viewport)

namespace df3d { namespace scene {

class DF3D_DLL Camera
{
    //! Position of the camera in the world.
    glm::vec3 m_position;
    //! Right vector.
    glm::vec3 m_right;
    //! Up vector.
    glm::vec3 m_up;
    //! Direction vector.
    glm::vec3 m_dir;

    float m_yaw, m_pitch, m_roll;

    glm::mat4 m_worldToCamera;
    glm::mat4 m_projectionMatrix;

    shared_ptr<render::Viewport> m_viewport;

    //! Field of view.
    float m_fov;

    //! Near Z-clipping plane position.
    float m_nearZ;
    //! Far Z-clipping plane position.
    float m_farZ;

    bool m_viewMatrixDirty;
    bool m_projectionMatrixDirty;
    void buildCamMatrix();

public:
    /** 
      * Initializes camera with position,
      * field of view, near and far clipping planes
      */
    Camera(const glm::vec3 &position = glm::vec3(),
        float fov = 60.f,
        float nearZ = 1.f,
        float farZ = 8000.f);
    ~Camera();

    void setPosition(const glm::vec3 &pos);
    glm::vec3 getPosition() const { return m_position; }
    
    void moveForward(float step);
    void moveBackward(float step);
    void moveLeft(float step);
    void moveRight(float step);

    void move(const glm::vec3 &vec);
    void lookAt(const glm::vec3 &pt);

    void setFov(float fov);

    void setRotation(float yaw, float pitch, float roll, bool rads = false);
    glm::vec3 getRotation(bool rads = false) const;

    const glm::mat4 &getMatrix();

    const glm::vec3 &getRight();
    const glm::vec3 &getUp();
    const glm::vec3 &getDir();

    float getFov() const { return m_fov; }
    float getNearZ() const { return m_nearZ; }
    float getFarZ() const { return m_farZ; }

    const glm::mat4 &getProjectionMatrix();

    glm::vec3 screenToViewPoint(float x, float y, float z = 0.0f);
    glm::vec2 worldToScreenPoint(const glm::vec3 &world);
};

} }