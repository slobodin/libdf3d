#pragma once

#include <CEGUI/RenderTarget.h>
#include <CEGUI/GeometryBuffer.h>
#include "CeguiRendererImpl.h"
#include <base/Controller.h>
#include <render/RenderManager.h>
#include <render/Renderer.h>
#include <render/RenderTarget.h>

FWD_MODULE_CLASS(render, RenderTarget)

namespace df3d { namespace gui { namespace cegui_impl {

template <typename T = CEGUI::RenderTarget>
class CeguiRenderTargetImpl : public T
{
protected:
    CEGUI::Rectf m_area = { 0.0f, 0.0f, 0.0f, 0.0f };
    CeguiRendererImpl &m_owner;

    // Subclasses should init it properly.
    shared_ptr<render::RenderTarget> m_rt;

    mutable bool m_matrixDirty = true;
    mutable glm::mat4 m_projMatrix;
    mutable glm::mat4 m_cameraMatrix;
    mutable float m_viewDistance = 0.0f;

    // Tangent of the y FOV half-angle.
    const float m_yfov_tan = 0.267949192431123f;

    void updateMatrices()
    {
        if (m_matrixDirty)
        {
            const float w = m_area.getWidth();
            const float h = m_area.getHeight();

            // We need to check if width or height are zero and act accordingly to prevent running into issues
            // with divisions by zero which would lead to undefined values, as well as faulty clipping planes
            // This is mostly important for avoiding asserts
            const bool widthAndHeightNotZero = (w != 0.0f) && (h != 0.0f);

            const float aspect = widthAndHeightNotZero ? w / h : 1.0f;
            const float midx = widthAndHeightNotZero ? w * 0.5f : 0.5f;
            const float midy = widthAndHeightNotZero ? h * 0.5f : 0.5f;
            m_viewDistance = midx / (aspect * m_yfov_tan);

            glm::vec3 eye = glm::vec3(midx, midy, float(-m_viewDistance));
            glm::vec3 center = glm::vec3(midx, midy, 1);
            glm::vec3 up = glm::vec3(0, -1, 0);

            m_projMatrix = glm::perspective(30.f, aspect, float(m_viewDistance * 0.5), float(m_viewDistance * 2.0));
            m_cameraMatrix = glm::lookAt(eye, center, up);

            m_matrixDirty = false;
        }
    }

public:
    CeguiRenderTargetImpl(CeguiRendererImpl &owner)
        : m_owner(owner)
    {

    }

    virtual ~CeguiRenderTargetImpl()
    {

    }

    void draw(const CEGUI::GeometryBuffer &buffer)
    {
        buffer.draw();
    }

    void draw(const CEGUI::RenderQueue &queue)
    {
        queue.draw();
    }

    void setArea(const CEGUI::Rectf &area)
    {
        m_area = area;
        m_matrixDirty = true;

        // TODO: create new texture and render target.

        CEGUI::RenderTargetEventArgs args(this);
        T::fireEvent(RenderTarget::EventAreaChanged, args);
    }

    const CEGUI::Rectf& getArea() const
    {
        return m_area;
    }

    void activate()
    {
        assert(m_rt);

        updateMatrices();
        g_renderManager->setupViewport(m_rt);
        g_renderManager->getRenderer()->setCameraMatrix(m_cameraMatrix);
        g_renderManager->getRenderer()->setProjectionMatrix(m_projMatrix);
    }

    void deactivate()
    {
        m_rt->unbind();
    }

    void unprojectPoint(const CEGUI::GeometryBuffer &buff, const CEGUI::Vector2f &p_in, CEGUI::Vector2f &p_out) const
    {

    }
};

} } }
