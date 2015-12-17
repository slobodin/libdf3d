#include "Camera.h"

#include <base/EngineController.h>
#include <components/TransformComponent.h>
#include <render/RendererBackend.h>
#include <render/Viewport.h>
#include <render/RenderTargetScreen.h>
#include <render/RenderManager.h>

namespace df3d {

void Camera::buildProjectionMatrix()
{
    const auto &vp = svc().renderManager().getScreenRenderTarget()->getViewport();

    float w = (float)vp.width();
    float h = (float)vp.height();

    m_projectionMatrix = glm::perspective(glm::radians(m_fov), w / h, m_nearZ, m_farZ);
    m_currentViewport = vp;

    buildFrustum();
}

void Camera::buildFrustum()
{
    m_frustum = Frustum(getProjectionMatrix() * getViewMatrix());
}

void Camera::buildCameraMatrix()
{
    // Rotation -> Translation
    m_transformation = glm::translate(m_position) * glm::toMat4(m_orientation);
    // View matrix is the inverse of camera transformation matrix.
    // FIXME:
    // TODO_ecs:
    // Could use transpose(rotation) and inverse translation vector instead computing inverse matrix.
    m_worldToCamera = glm::inverse(m_transformation);

    buildFrustum();
}

Camera::Camera(const glm::vec3 &position, float fov, float nearZ, float farZ)
    : m_fov(fov),
    m_nearZ(nearZ),
    m_farZ(farZ),
    m_position(position)
{
    buildCameraMatrix();
    buildProjectionMatrix();
}

Camera::~Camera()
{

}

void Camera::setFov(float fov)
{
    m_fov = fov;
    buildProjectionMatrix();
}

void Camera::setPosition(const glm::vec3 &newPosition)
{
    m_position = newPosition;
    buildCameraMatrix();
}

void Camera::setOrientation(const glm::quat &q)
{
    m_orientation = q;
    buildCameraMatrix();
}

void Camera::setOrientation(const glm::vec3 &eulerAngles, bool rads)
{
    glm::vec3 v = rads ? eulerAngles : glm::radians(eulerAngles);

    setOrientation(glm::quat(v));
}

const glm::mat4 &Camera::getViewMatrix() const
{
    return m_worldToCamera;
}

const glm::mat4 &Camera::getProjectionMatrix() const
{
    return m_projectionMatrix;
}

const glm::vec3& Camera::getPosition() const
{
    return m_position;
}

const glm::quat& Camera::getOrientation() const
{
    return m_orientation;
}

const glm::mat4& Camera::getWorldTransform() const
{
    return m_transformation;
}

glm::vec3 Camera::getRight() const
{
    return glm::normalize(glm::vec3(glm::column(m_transformation, 0)));
}

glm::vec3 Camera::getUp() const
{
    return glm::normalize(glm::vec3(glm::column(m_transformation, 1)));
}

glm::vec3 Camera::getDir() const
{
    // Could be this:
    //auto dir = glm::normalize(transform()->getTransformation() * glm::vec4(0.0f, 0.0f, -1.0f, 0.0f));
    return -glm::normalize(glm::vec3(glm::column(m_transformation, 2)));
}

const Frustum &Camera::getFrustum()
{
    return m_frustum;
}

glm::vec3 Camera::screenToViewPoint(float x, float y, float z)
{
    const auto &vp = svc().renderManager().getScreenRenderTarget()->getViewport();

    float w = (float)vp.width();
    float h = (float)vp.height();
    //float z = svc().renderManager().getRenderer()->readDepthBuffer(x, h - y - 1);

    glm::vec4 viewport = glm::vec4(0.0f, 0.0f, w, h);
    auto viewPos = glm::unProject(glm::vec3(x, h - y - 1, z), getViewMatrix(), getProjectionMatrix(), viewport);

    return viewPos;
}

glm::vec3 Camera::worldToScreenPoint(const glm::vec3 &world)
{
    const auto &vp = svc().renderManager().getScreenRenderTarget()->getViewport();

    float w = (float)vp.width();
    float h = (float)vp.height();

    glm::vec4 viewport = glm::vec4(0.0f, 0.0f, w, h);
    auto screenPt = glm::project(world, getViewMatrix(), getProjectionMatrix(), viewport);

    return screenPt;
}

}
