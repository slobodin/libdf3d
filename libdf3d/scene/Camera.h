#pragma once

#include "Node.h"
#include <render/Viewport.h>

namespace df3d { namespace scene {

class DF3D_DLL Camera : public Node
{
    glm::mat4 m_worldToCamera;
    glm::mat4 m_projectionMatrix;

    //! Field of view.
    float m_fov;
    //! Near Z-clipping plane position.
    float m_nearZ;
    //! Far Z-clipping plane position.
    float m_farZ;

    //! Cached copy of the current screen viewport.
    render::Viewport m_currentViewport;

    bool m_viewMatrixDirty = true;
    bool m_projectionMatrixDirty = true;
    void buildViewMatrix();
    void buildProjectionMatrix();

    void onComponentEvent(const components::NodeComponent *who, components::ComponentEvent ev) override;

public:
    /** 
      * Initializes camera with position,
      * field of view, near and far clipping planes
      */
    Camera(const glm::vec3 &position = glm::vec3(),
        float fov = 60.f,
        float nearZ = 0.1f,
        float farZ = 3000.f);
    ~Camera();

    void setFov(float fov);
    void setViewMatrix(const glm::mat4 &viewm);
    
    const glm::mat4 &getViewMatrix();
    const glm::mat4 &getProjectionMatrix();
    glm::vec3 getRight();
    glm::vec3 getUp();
    glm::vec3 getDir();

    float getFov() const { return m_fov; }
    float getNearZ() const { return m_nearZ; }
    float getFarZ() const { return m_farZ; }

    glm::vec3 screenToViewPoint(float x, float y, float z = 0.0f);
    glm::vec3 worldToScreenPoint(const glm::vec3 &world);
};

} }
