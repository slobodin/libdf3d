#include "df3d_pch.h"
#include "Camera.h"

#include <base/Controller.h>
#include <render/RenderManager.h>
#include <render/Renderer.h>
#include <render/Viewport.h>

namespace df3d { namespace scene {

void Camera::buildCamMatrix()
{
    // Get inverse orientation.
    auto orientation = glm::angleAxis(glm::radians(-m_pitch), glm::vec3(1.0f, 0.0f, 0.0f)) * glm::angleAxis(glm::radians(-m_yaw), glm::vec3(0.0f, 1.0f, 0.0f));
    // Apply inverse translation, then inverse orientation.
    m_worldToCamera = glm::translate(glm::toMat4(orientation), -m_position);

    // Compute basis vectors.
    m_up = glm::vec3(0.0f, 1.0f, 0.0f);
    m_dir = glm::vec3(0.0f, 0.0f, -1.0f);
    m_right = glm::vec3(1.0f, 0.0f, 0.0f);

    // Get camera world orientation. Actually we should get inverse of m_worldToCamera matrix,
    // but we need only rotation component, so we can transpose matrix (rotation matrices are orthogonal).
    auto cameraWorldOrientation = glm::transpose(m_worldToCamera);

    // Rotate basis vectors.
    m_up = glm::vec3(cameraWorldOrientation * glm::vec4(m_up, 0.0f));
    m_dir = glm::vec3(cameraWorldOrientation * glm::vec4(m_dir, 0.0f));
    m_right = glm::vec3(cameraWorldOrientation * glm::vec4(m_right, 0.0f));

    // Normalize.
    // FIXME:
    // Should we make this basis orthogonal?
    glm::normalize(m_up);
    glm::normalize(m_dir);
    glm::normalize(m_right);

    m_viewMatrixDirty = false;
}

Camera::Camera(const glm::vec3 &position, float fov, float nearZ, float farZ)
    : m_position(position),
    m_right(1.0f, 0.0f, 0.0f),
    m_up(0.0f, 1.0f, 0.0f),
    m_dir(0.0f, 0.0f, -1.0f),       // Right-handed OpenGL system.
    m_yaw(0), 
    m_pitch(0),
    m_roll(0),
    m_fov(fov),
    m_nearZ(nearZ),
    m_farZ(farZ),
    m_projectionMatrixDirty(true)
{
    buildCamMatrix();

    m_viewport = g_renderManager->getViewport();
}

Camera::~Camera()
{
}

void Camera::setPosition(const glm::vec3 &pos)
{
    m_position = pos;
    m_viewMatrixDirty = true;
}

void Camera::moveForward(float step)
{
    m_position += m_dir * step;
    m_viewMatrixDirty = true;
}

void Camera::moveBackward(float step)
{
    m_position -= m_dir * step;
    m_viewMatrixDirty = true;
}

void Camera::moveLeft(float step)
{
    m_position -= m_right * step;
    m_viewMatrixDirty = true;
}

void Camera::moveRight(float step)
{
    m_position += m_right * step;
    m_viewMatrixDirty = true;
}

void Camera::move(const glm::vec3 &vec)
{
    m_position += vec;
    m_viewMatrixDirty = true;
}

void Camera::lookAt(const glm::vec3 &pt)
{
    m_up = glm::vec3(0.0f, 1.0f, 0.0f);
    m_dir = pt - m_position;
    m_right = glm::cross(m_up, m_dir);

    m_right = glm::normalize(m_right);
    m_dir = glm::normalize(m_dir);

    m_up = glm::cross(m_dir, m_right);
    m_up = glm::normalize(m_up);

    m_worldToCamera = glm::lookAt(m_position, m_position + m_dir, m_up);
    m_viewMatrixDirty = false;
}

void Camera::setFov(float fov)
{
    m_fov = fov;
    m_projectionMatrixDirty = true;
}

void Camera::setRotation(float yaw, float pitch, float roll, bool rads)
{
    m_yaw = yaw;
    m_pitch = pitch;
    m_roll = roll;

    if (rads)
    {
        m_yaw = glm::degrees(m_yaw);
        m_pitch = glm::degrees(m_pitch);
        m_roll = glm::degrees(m_roll);
    }

    m_viewMatrixDirty = true;
}

glm::vec3 Camera::getRotation(bool rads) const
{
    if (rads)
        return glm::vec3(glm::radians(m_pitch), glm::radians(m_yaw), glm::radians(m_roll));
    else
        return glm::vec3(m_pitch, m_yaw, m_roll);
}

const glm::mat4 &Camera::getMatrix()
{
    if (m_viewMatrixDirty)
        buildCamMatrix();

    return m_worldToCamera;
}

const glm::vec3 &Camera::getRight()
{
    if (m_viewMatrixDirty)
        buildCamMatrix();

    return m_right;
}

const glm::vec3 &Camera::getUp()
{
    if (m_viewMatrixDirty)
        buildCamMatrix();

    return m_up;
}

const glm::vec3 &Camera::getDir()
{
    if (m_viewMatrixDirty)
        buildCamMatrix();

    return m_dir;
}

const glm::mat4 &Camera::getProjectionMatrix()
{
    if (m_projectionMatrixDirty)
    {
        float w = (float)m_viewport->width();
        float h = (float)m_viewport->height();

        m_projectionMatrix = glm::perspective(glm::radians(m_fov), w / h, m_nearZ, m_farZ);
        m_projectionMatrixDirty = false;
    }

    return m_projectionMatrix;
}

glm::vec3 Camera::screenToViewPoint(float x, float y, float z)
{
    float w = (float)g_renderManager->getViewport()->width();
    float h = (float)g_renderManager->getViewport()->height();
    //float z = g_renderManager->getRenderer()->readDepthBuffer(x, h - y - 1);

    glm::vec4 viewport = glm::vec4(0.0f, 0.0f, w, h);
    auto viewPos = glm::unProject(glm::vec3(x, h - y - 1, z), getMatrix(), getProjectionMatrix(), viewport);

    return viewPos;
}

glm::vec2 Camera::worldToScreenPoint(const glm::vec3 &world)
{
    float w = (float)g_renderManager->getViewport()->width();
    float h = (float)g_renderManager->getViewport()->height();

    glm::vec4 viewport = glm::vec4(0.0f, 0.0f, w, h);
    auto screenPt = glm::project(world, getMatrix(), getProjectionMatrix(), viewport);

    return glm::vec2(screenPt);
}

} }