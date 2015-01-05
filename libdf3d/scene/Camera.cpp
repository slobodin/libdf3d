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
    if (!m_viewMatrixDirty)
        return;

    // View matrix is the inverse of camera transformation matrix.
    // FIXME:
    // Could use transpose(rotation) and inverse translation vector instead computing inverse matrix.
    m_worldToCamera = glm::inverse(transform()->getTransformation());
    m_viewMatrixDirty = false;
}

void Camera::buildProjectionMatrix()
{
    const auto &vp = g_renderManager->getScreenRenderTarget()->getViewport();

    if (m_currentViewport != vp)
        m_projectionMatrixDirty = true;

    if (!m_projectionMatrixDirty)
        return;

    float w = (float)vp.width();
    float h = (float)vp.height();

    m_projectionMatrix = glm::perspective(glm::radians(m_fov), w / h, m_nearZ, m_farZ);
    m_projectionMatrixDirty = false;
    m_currentViewport = vp;
}

void Camera::onComponentEvent(const components::NodeComponent *who, components::ComponentEvent ev)
{
    if (ev == components::ComponentEvent::TRANFORM_CHANGED)
        m_viewMatrixDirty = true;
}

Camera::Camera(const glm::vec3 &position, float fov, float nearZ, float farZ)
    : Node("__libdf3d_CameraNode"),
    m_fov(fov),
    m_nearZ(nearZ),
    m_farZ(farZ)
{
    attachComponent(make_shared<components::TransformComponent>());
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

void Camera::setViewMatrix(const glm::mat4 &viewm)
{
    m_worldToCamera = viewm;
    m_viewMatrixDirty = false;
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
    return glm::normalize(glm::vec3(glm::column(transform()->getTransformation(), 0)));
}

glm::vec3 Camera::getUp()
{
    return glm::normalize(glm::vec3(glm::column(transform()->getTransformation(), 1)));
}

glm::vec3 Camera::getDir()
{
    // Could be this:
    //auto dir = glm::normalize(transform()->getTransformation() * glm::vec4(0.0f, 0.0f, -1.0f, 0.0f));
    return -glm::normalize(glm::vec3(glm::column(transform()->getTransformation(), 2)));
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

glm::vec3 Camera::worldToScreenPoint(const glm::vec3 &world)
{
    const auto &vp = g_renderManager->getScreenRenderTarget()->getViewport();

    float w = (float)vp.width();
    float h = (float)vp.height();

    glm::vec4 viewport = glm::vec4(0.0f, 0.0f, w, h);
    auto screenPt = glm::project(world, getViewMatrix(), getProjectionMatrix(), viewport);

    return screenPt;
}

} }
