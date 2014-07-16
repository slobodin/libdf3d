#include "df3d_pch.h"
#include "FontFace.h"

namespace df3d { namespace gui {

FontFace::FontFace()
{
}

FontFace::~FontFace()
{
    if (m_face)
        FT_Done_Face(m_face);
}

bool FontFace::init()
{
    return m_face != nullptr;
}

} }