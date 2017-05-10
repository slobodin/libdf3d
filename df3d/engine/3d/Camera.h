#pragma once

#include <df3d/lib/math/Frustum.h>
#include <df3d/lib/math/MathUtils.h>
#include <df3d/engine/render/Viewport.h>

namespace df3d {

class Camera : NonCopyable  // Should be polymorphic.
{
    glm::mat4 m_worldToCamera;
    glm::mat4 m_projectionMatrix;

    //! Field of view.
    float m_fov;
    //! Near Z-clipping plane position.
    float m_nearZ;
    //! Far Z-clipping plane position.
    float m_farZ;

    //! View frustum.
    Frustum m_frustum;

    //! Camera position.
    glm::vec3 m_position;
    //! Camera orientation.
    glm::quat m_orientation;
    //! World transformation.
    glm::mat4 m_transformation;

    void buildProjectionMatrix();
    void buildFrustum();
    void buildCameraMatrix();

public:
    static const float DEFAULT_NEAR_Z;
    static const float DEFAULT_FAR_Z;
    static const float DEFAULT_FOV;

    /** 
      * Initializes camera with position,
      * field of view, near and far clipping planes
      */
    Camera(const glm::vec3 &position, float fov, float nearZ, float farZ);
    virtual ~Camera();

    void setFov(float fov);
    void setPosition(const glm::vec3 &newPosition);
    void setOrientation(const glm::quat &q);
    void setOrientation(const glm::vec3 &eulerAngles);

    const glm::mat4& getViewMatrix() const;
    const glm::mat4& getProjectionMatrix() const;
    const glm::vec3& getPosition() const;
    const glm::quat& getOrientation() const;
    const glm::mat4& getWorldTransform() const;
    glm::vec3 getRight() const;
    glm::vec3 getUp() const;
    glm::vec3 getDir() const;

    float getFov() const { return m_fov; }
    float getNearZ() const { return m_nearZ; }
    float getFarZ() const { return m_farZ; }

    const Frustum& getFrustum();

    glm::vec3 screenToViewPoint(float x, float y, float z = 0.0f) const;
    glm::vec3 worldToScreenPoint(const glm::vec3 &world) const;
    Ray createPickingRay(float x, float y);

    static shared_ptr<Camera> createDefault(const glm::vec3 &pos = {})
    {
        return make_shared<Camera>(pos, DEFAULT_FOV, DEFAULT_NEAR_Z, DEFAULT_FAR_Z);
    }
};

}
