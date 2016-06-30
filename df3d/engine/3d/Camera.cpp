#include "Camera.h"

#include <df3d/engine/EngineController.h>

namespace df3d {

const float Camera::DEFAULT_NEAR_Z = 1.0f;
const float Camera::DEFAULT_FAR_Z = 5000.0f;
const float Camera::DEFAULT_FOV = 60.0f;

void Camera::buildProjectionMatrix()
{
    const auto &screenSize = svc().getScreenSize();

    m_projectionMatrix = glm::perspective(glm::radians(m_fov), screenSize.x / screenSize.y, m_nearZ, m_farZ);

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

void Camera::setOrientation(const glm::vec3 &eulerAngles)
{
    setOrientation(glm::quat(glm::radians(eulerAngles)));
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

glm::vec3 Camera::screenToViewPoint(float x, float y, float z) const
{
    const auto &screenSize = svc().getScreenSize();

    glm::vec4 viewport = glm::vec4(0.0f, 0.0f, screenSize.x, screenSize.y);
    auto viewPos = glm::unProject(glm::vec3(x, screenSize.y - y - 1, z), getViewMatrix(), getProjectionMatrix(), viewport);

    return viewPos;
}

glm::vec3 Camera::worldToScreenPoint(const glm::vec3 &world) const
{
    const auto &screenSize = svc().getScreenSize();

    glm::vec4 viewport = glm::vec4(0.0f, 0.0f, screenSize.x, screenSize.y);
    auto screenPt = glm::project(world, getViewMatrix(), getProjectionMatrix(), viewport);

    return screenPt;
}

utils::math::Ray Camera::createPickingRay(float x, float y)
{
    auto nearPt = screenToViewPoint(x, y, 0.0f);
    auto farPt = screenToViewPoint(x, y, 1.0f);

    utils::math::Ray result;
    result.origin = nearPt;
    result.dir = glm::normalize(farPt - nearPt);

    return result;
}

}
