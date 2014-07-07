#pragma once

namespace df3d { namespace render {

class RenderTarget : boost::noncopyable
{
public:
    RenderTarget() { }
    virtual ~RenderTarget() { }

    virtual void bind() = 0;
    virtual void unbind() = 0;

    virtual int getWidth() const = 0;
    virtual int getHeight() const = 0;
};

} }