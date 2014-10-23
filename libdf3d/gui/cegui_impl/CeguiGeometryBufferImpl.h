#pragma once

#include <CEGUI/GeometryBuffer.h>

namespace df3d { namespace gui { namespace cegui_impl {

class CeguiGeometryBufferImpl : public CEGUI::GeometryBuffer
{
public:
    CeguiGeometryBufferImpl();
    virtual ~CeguiGeometryBufferImpl();

    virtual void draw() const;
    virtual void setTranslation(const CEGUI::Vector3f &v);
    virtual void setRotation(const CEGUI::Quaternion &r);
    virtual void setPivot(const CEGUI::Vector3f &p);
    virtual void setClippingRegion(const CEGUI::Rectf &region);
    virtual void appendVertex(const CEGUI::Vertex &vertex);
    virtual void appendGeometry(const CEGUI::Vertex *const vbuff, CEGUI::uint vertex_count);
    virtual void setActiveTexture(CEGUI::Texture *texture);
    virtual void reset();
    virtual CEGUI::Texture* getActiveTexture() const;
    virtual CEGUI::uint getVertexCount() const;
    virtual CEGUI::uint getBatchCount() const;
    virtual void setRenderEffect(CEGUI::RenderEffect *effect);
    virtual CEGUI::RenderEffect* getRenderEffect();
    void setClippingActive(const bool active);
    bool isClippingActive() const;
};

} } }
