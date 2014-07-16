#include "df3d_pch.h"
#include "TextRenderer.h"

// TEST CODE

#include "Texture.h"
#include <gui/FontFace.h>
#include <base/Controller.h>
#include <resources/ResourceManager.h>

#include <ft2build.h>
#include FT_FREETYPE_H

namespace df3d { namespace render {

TextRenderer::TextRenderer(const char *fontPath)
{
    auto font = g_resourceManager->getResource<gui::FontFace>(fontPath);
    if (!font)
    {
        base::glog << "Failed to initialize text renderer. Font is invalid" << base::logwarn;
        return;
    }

    m_font = font;
}

void TextRenderer::drawText(const char *text)
{
    if (!m_font)
        return;
}

} }