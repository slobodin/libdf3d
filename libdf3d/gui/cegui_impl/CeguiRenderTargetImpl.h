#pragma once

#include <CEGUI/RenderTarget.h>
#include <CEGUI/GeometryBuffer.h>
#include <cegui/Exceptions.h>
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

        CEGUI::RenderTargetEventArgs args(this);
        T::fireEvent(RenderTarget::EventAreaChanged, args);
    }

    const CEGUI::Rectf& getArea() const
    {
        return m_area;
    }

    void activate()
    {
        m_rt->bind();

        g_renderManager->getRenderer()->setCameraMatrix(glm::mat4(1.0f));
        // TODO:
        // Cache this matrix.
        g_renderManager->getRenderer()->setProjectionMatrix(glm::ortho(m_area.left(), m_area.right(), m_area.bottom(), m_area.top()));

        m_owner.setActiveRenderTarget(this);
    }

    void deactivate()
    {
        m_rt->unbind();
    }

    void unprojectPoint(const CEGUI::GeometryBuffer &buff, const CEGUI::Vector2f &p_in, CEGUI::Vector2f &p_out) const
    {
        using namespace CEGUI;
        CEGUI_THROW(InvalidRequestException("unprojectPoint for libdf3d is not implemented."));
    }
};

} } }
