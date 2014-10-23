#include "df3d_pch.h"

#include "CeguiRendererImpl.h"

namespace df3d { namespace gui { namespace cegui_impl {

using namespace CEGUI;

//String CeguiRendererImpl::d_rendererID("CEGUI libdf3d renderer.");

CeguiRendererImpl& CeguiRendererImpl::bootstrapSystem(const int abi)
{
    System::performVersionTest(CEGUI_VERSION_ABI, abi, CEGUI_FUNCTION_NAME);

    if (System::getSingletonPtr())
        CEGUI_THROW(InvalidRequestException(
            "CEGUI::System object is already initialised."));

    CeguiRendererImpl &renderer = create();
	
    DefaultResourceProvider *rp(CEGUI_NEW_AO DefaultResourceProvider());

	// TODO: Create image codec?
	// NullImageCodec& ic = createNullImageCodec();
    System::create(renderer, rp, static_cast<XMLParser*>(0), 0);

    return renderer;
}

void CeguiRendererImpl::destroySystem()
{
    System *sys;
    if (!(sys = System::getSingletonPtr()))
        CEGUI_THROW(InvalidRequestException(
            "CEGUI::System object is not created or was already destroyed."));

    CeguiRendererImpl *renderer = static_cast<CeguiRendererImpl*>(sys->getRenderer());
    ResourceProvider *rp = sys->getResourceProvider();

    //ImageCodec* ic = &(sys->getImageCodec());

    System::destroy();
    // ImageCodec used is currently the system default, so we do not destroy
    // it here (since System already did that).
    //CEGUI_DELETE_AO ic;
	CEGUI_DELETE_AO rp;
    destroy(*renderer);
}

CeguiRendererImpl& CeguiRendererImpl::create(const int abi)
{
    System::performVersionTest(CEGUI_VERSION_ABI, abi, CEGUI_FUNCTION_NAME);

    return *CEGUI_NEW_AO CeguiRendererImpl();
}

void CeguiRendererImpl::destroy(CeguiRendererImpl &renderer)
{
    CEGUI_DELETE_AO &renderer;
}

CEGUI::RenderTarget& CeguiRendererImpl::getDefaultRenderTarget()
{

}

CEGUI::GeometryBuffer& CeguiRendererImpl::createGeometryBuffer()
{

}

void CeguiRendererImpl::destroyGeometryBuffer(const CEGUI::GeometryBuffer &buffer)
{

}

void CeguiRendererImpl::destroyAllGeometryBuffers()
{

}

CEGUI::TextureTarget* CeguiRendererImpl::createTextureTarget()
{

}

void CeguiRendererImpl::destroyTextureTarget(CEGUI::TextureTarget *target)
{

}

void CeguiRendererImpl::destroyAllTextureTargets()
{

}

CEGUI::Texture& CeguiRendererImpl::createTexture(const CEGUI::String &name)
{

}

CEGUI::Texture& CeguiRendererImpl::createTexture(const CEGUI::String &name, const CEGUI::String &filename, const CEGUI::String &resourceGroup)
{

}

CEGUI::Texture& CeguiRendererImpl::createTexture(const CEGUI::String &name, const CEGUI::Sizef &size)
{

}

void CeguiRendererImpl::destroyTexture(CEGUI::Texture &texture)
{

}

void CeguiRendererImpl::destroyTexture(const CEGUI::String &name)
{

}

void CeguiRendererImpl::destroyAllTextures()
{

}

CEGUI::Texture& CeguiRendererImpl::getTexture(const CEGUI::String &name) const
{

}

bool CeguiRendererImpl::isTextureDefined(const CEGUI::String &name) const
{

}

void CeguiRendererImpl::beginRendering()
{

}

void CeguiRendererImpl::endRendering()
{

}

void CeguiRendererImpl::setDisplaySize(const CEGUI::Sizef &sz)
{

}

const CEGUI::Sizef& CeguiRendererImpl::getDisplaySize() const
{

}

const CEGUI::Vector2f& CeguiRendererImpl::getDisplayDPI() const
{

}

CEGUI::uint CeguiRendererImpl::getMaxTextureSize() const
{

}

const CEGUI::String& CeguiRendererImpl::getIdentifierString() const
{

}

} } }