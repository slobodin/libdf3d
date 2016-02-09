#pragma once

#include "Frustum.h"
#include <libdf3d/render/Viewport.h>
#include <libdf3d/utils/MathUtils.h>

namespace df3d {

class DF3D_DLL Camera : utils::NonCopyable  // Should be polymorphic.
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
    /** 
      * Initializes camera with position,
      * field of view, near and far clipping planes
      */
    Camera(const glm::vec3 &position = glm::vec3(),
           float fov = 60.f,
           float nearZ = 0.1f,
           float farZ = 5000.f);
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
    utils::math::Ray createPickingRay(float x, float y);
};

}
