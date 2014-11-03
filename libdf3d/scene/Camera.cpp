#include "df3d_pch.h"
#include "Camera.h"

#include <base/Controller.h>
#include <components/TransformComponent.h>
#include <render/RenderManager.h>
#include <render/Renderer.h>
#include <render/Viewport.h>
#include <render/RenderTargetScreen.h>

namespace df3d { namespace scene {

void Camera::buildViewMatrix()
{
    //// Get inverse orientation.
    //auto orientation = glm::angleAxis(glm::radians(-m_pitch), glm::vec3(1.0f, 0.0f, 0.0f)) * glm::angleAxis(glm::radians(-m_yaw), glm::vec3(0.0f, 1.0f, 0.0f));
    //// Apply inverse translation, then inverse orientation.
    //m_worldToCamera = glm::translate(glm::toMat4(orientation), -m_position);

    //// Compute basis vectors.
    //m_up = glm::vec3(0.0f, 1.0f, 0.0f);
    //m_dir = glm::vec3(0.0f, 0.0f, -1.0f);
    //m_right = glm::vec3(1.0f, 0.0f, 0.0f);

    //// Get camera world orientation. Actually we should get inverse of m_worldToCamera matrix,
    //// but we need only rotation component, so we can transpose matrix (rotation matrices are orthogonal).
    //auto cameraWorldOrientation = glm::transpose(m_worldToCamera);

    //// Rotate basis vectors.
    //m_up = glm::vec3(cameraWorldOrientation * glm::vec4(m_up, 0.0f));
    //m_dir = glm::vec3(cameraWorldOrientation * glm::vec4(m_dir, 0.0f));
    //m_right = glm::vec3(cameraWorldOrientation * glm::vec4(m_right, 0.0f));

    //// Normalize.
    //// FIXME:
    //// Should we make this basis orthogonal?
    //glm::normalize(m_up);
    //glm::normalize(m_dir);
    //glm::normalize(m_right);

    //m_viewMatrixDirty = false;

    if (!m_viewMatrixDirty)
        return;

    m_worldToCamera = glm::inverse(transform()->getTransformation());
    m_viewMatrixDirty = false;
}

void Camera::buildProjectionMatrix()
{
    if (!m_projectionMatrixDirty)
        return;

    const auto &vp = g_renderManager->getScreenRenderTarget()->getViewport();

    float w = (float)vp.width();
    float h = (float)vp.height();

    m_projectionMatrix = glm::perspective(glm::radians(m_fov), w / h, m_nearZ, m_farZ);
    m_projectionMatrixDirty = false;
}

void Camera::onComponentEvent(const components::NodeComponent *who, components::Event ev)
{
    if (ev == components::CE_TRANFORM_CHANGED)
        m_viewMatrixDirty = true;
}

Camera::Camera(const glm::vec3 &position, float fov, float nearZ, float farZ)
    : Node("__libdf3d_CameraNode"),
    m_fov(fov),
    m_nearZ(nearZ),
    m_farZ(farZ)
{
    transform()->setPosition(position);
}

Camera::~Camera()
{
}

void Camera::setFov(float fov)
{
    m_fov = fov;
    m_projectionMatrixDirty = true;
}

const glm::mat4 &Camera::getViewMatrix()
{
    buildViewMatrix();

    return m_worldToCamera;
}

const glm::mat4 &Camera::getProjectionMatrix()
{
    buildProjectionMatrix();

    return m_projectionMatrix;
}

glm::vec3 Camera::getRight()
{
    return glm::vec3(glm::row(transform()->getTransformation(), 0));
}

glm::vec3 Camera::getUp()
{
    return glm::vec3(glm::row(transform()->getTransformation(), 1));
}

glm::vec3 Camera::getDir()
{
    // FIXME:
    // !!!!
    auto dir = glm::vec3(glm::row(transform()->getTransformation(), 2));
    dir.z = -dir.z;
    return dir;
}

glm::vec3 Camera::screenToViewPoint(float x, float y, float z)
{
    const auto &vp = g_renderManager->getScreenRenderTarget()->getViewport();

    float w = (float)vp.width();
    float h = (float)vp.height();
    //float z = g_renderManager->getRenderer()->readDepthBuffer(x, h - y - 1);

    glm::vec4 viewport = glm::vec4(0.0f, 0.0f, w, h);
    auto viewPos = glm::unProject(glm::vec3(x, h - y - 1, z), getViewMatrix(), getProjectionMatrix(), viewport);

    return viewPos;
}

glm::vec2 Camera::worldToScreenPoint(const glm::vec3 &world)
{
    const auto &vp = g_renderManager->getScreenRenderTarget()->getViewport();

    float w = (float)vp.width();
    float h = (float)vp.height();

    glm::vec4 viewport = glm::vec4(0.0f, 0.0f, w, h);
    auto screenPt = glm::project(world, getViewMatrix(), getProjectionMatrix(), viewport);

    return glm::vec2(screenPt);
}

} }