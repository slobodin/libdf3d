#pragma once

#include <CEGUI/GeometryBuffer.h>
#include <CEGUI/Quaternion.h>

FWD_MODULE_CLASS(render, RenderOperation)

namespace df3d { namespace gui { namespace cegui_impl {

class CeguiTextureImpl;
class CeguiRendererImpl;

class CeguiGeometryBufferImpl : public CEGUI::GeometryBuffer
{
    CeguiRendererImpl &m_owner;

    scoped_ptr<render::RenderOperation> m_op;
    CEGUI::Vector3f m_translation = { 0.0f, 0.0f, 0.0f };
    CEGUI::Quaternion m_rotation = CEGUI::Quaternion::IDENTITY;
    CEGUI::Vector3f m_pivot = { 0.0f, 0.0f, 0.0f };
    mutable bool m_matrixDirty = true;

    glm::mat4 getMatrix() const;

    CEGUI::Rectf m_clippingRegion = { 0.0f, 0.0f, 0.0f, 0.0f };
    bool m_clippingActive = true;

    CeguiTextureImpl *m_texture = nullptr;

public:
    CeguiGeometryBufferImpl(CeguiRendererImpl &owner);
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
