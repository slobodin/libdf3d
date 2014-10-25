#pragma once

#include <CEGUI/RenderTarget.h>
#include <CEGUI/GeometryBuffer.h>
#include "CeguiRendererImpl.h"

namespace df3d { namespace gui { namespace cegui_impl {

template <typename T = CEGUI::RenderTarget>
class CeguiRenderTargetImpl : public T
{
protected:
    CEGUI::Rectf m_area = { 0.0f, 0.0f, 0.0f, 0.0f };
    CeguiRendererImpl &m_owner;

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

    }

    void deactivate()
    {

    }

    void unprojectPoint(const CEGUI::GeometryBuffer &buff, const CEGUI::Vector2f &p_in, CEGUI::Vector2f &p_out) const
    {

    }
};

} } }
