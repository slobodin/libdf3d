#include "df3d_pch.h"
#include "FontFace.h"

#include <render/Image.h>

#define STB_TRUETYPE_IMPLEMENTATION
#include <stb/stb_truetype.h>

namespace df3d { namespace gui {

FontFace::FontFace()
    : m_info(new stbtt_fontinfo())
{
}

FontFace::~FontFace()
{
}

bool FontFace::init()
{
    return m_data != nullptr;
}

shared_ptr<render::Image> FontFace::getGlyphImage(char c, int size)
{
    auto info = m_info.get();
    if (!info)
        return nullptr;

    // TODO: cache idx
    auto idx = stbtt_FindGlyphIndex(info, c);
    if (stbtt_IsGlyphEmpty(info, idx))
        return nullptr;

    auto scale = stbtt_ScaleForPixelHeight(info, (float)size);

    int width, height, xoff, yoff;
    auto imageData = stbtt_GetGlyphBitmap(info, scale, scale, idx, &width, &height, &xoff, &yoff);

    auto textureImage = make_shared<render::Image>();
    textureImage->setWithData(imageData, width, height, width * 1, render::Image::PF_GRAYSCALE);
    textureImage->setInitialized();

    stbtt_FreeBitmap(imageData, nullptr);

    return textureImage;
}

} }