#pragma once

#include <CEGUI/RenderTarget.h>

namespace df3d { namespace gui { namespace cegui_impl {

template <typename T = RenderTarget>
class CeguiRenderTargetImpl : public T
{
public:
    CeguiRenderTargetImpl()
    {

    }

    virtual ~CeguiRenderTargetImpl()
    {

    }

    void draw(const GeometryBuffer &buffer)
    {

    }

    void draw(const RenderQueue &queue)
    {

    }

    void setArea(const Rectf &area)
    {

    }

    const Rectf& getArea() const
    {

    }

    void activate()
    {

    }

    void deactivate()
    {

    }

    void unprojectPoint(const GeometryBuffer &buff, const Vector2f &p_in, Vector2f &p_out) const
    {

    }
};

} } }
