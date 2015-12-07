#pragma once

#include "Viewport.h"

namespace df3d {

class RenderTarget : utils::NonCopyable
{
protected:
    Viewport m_viewport;

public:
    RenderTarget() { }
    virtual ~RenderTarget() { }

    virtual void bind() = 0;
    virtual void unbind() = 0;

    virtual const Viewport& getViewport() const { return m_viewport; }
    virtual void setViewport(const Viewport &vp) { m_viewport = vp; }
};

}
