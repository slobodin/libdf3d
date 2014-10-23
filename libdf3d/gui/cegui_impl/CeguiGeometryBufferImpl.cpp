#include "df3d_pch.h"

#include "CeguiGeometryBufferImpl.h"

namespace df3d { namespace gui { namespace cegui_impl {

using namespace CEGUI;

CeguiGeometryBufferImpl::CeguiGeometryBufferImpl()
{

}

CeguiGeometryBufferImpl::~CeguiGeometryBufferImpl()
{

}

void CeguiGeometryBufferImpl::draw() const
{

}

void CeguiGeometryBufferImpl::setTranslation(const Vector3f &v)
{

}

void CeguiGeometryBufferImpl::setRotation(const Quaternion &r)
{

}

void CeguiGeometryBufferImpl::setPivot(const Vector3f &p)
{

}

void CeguiGeometryBufferImpl::setClippingRegion(const Rectf &region)
{

}

void CeguiGeometryBufferImpl::appendVertex(const Vertex &vertex)
{

}

void CeguiGeometryBufferImpl::appendGeometry(const Vertex *const vbuff, uint vertex_count)
{

}

void CeguiGeometryBufferImpl::setActiveTexture(Texture *texture)
{

}

void CeguiGeometryBufferImpl::reset()
{

}

Texture* CeguiGeometryBufferImpl::getActiveTexture() const
{
    return nullptr;
}

uint CeguiGeometryBufferImpl::getVertexCount() const
{
    return 0;
}

uint CeguiGeometryBufferImpl::getBatchCount() const
{
    return 0;
}

void CeguiGeometryBufferImpl::setRenderEffect(RenderEffect *effect)
{

}

RenderEffect* CeguiGeometryBufferImpl::getRenderEffect()
{
    return nullptr;
}

void CeguiGeometryBufferImpl::setClippingActive(const bool active)
{

}

bool CeguiGeometryBufferImpl::isClippingActive() const
{
    return false;
}

} } }