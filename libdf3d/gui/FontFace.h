#pragma once

#include <resources/Resource.h>

#include <ft2build.h>
#include FT_FREETYPE_H

namespace df3d { namespace gui {

class FontFace : public resources::Resource
{
    FT_Face m_face = nullptr;

public:
    FontFace();
    ~FontFace();

    bool init();

    void setFtFace(FT_Face face) { m_face = face; }
};

} }