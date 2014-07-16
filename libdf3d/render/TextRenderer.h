#pragma once

FWD_MODULE_CLASS(gui, FontFace)

namespace df3d { namespace render {

class Texture;

class DF3D_DLL TextRenderer
{
    shared_ptr<Texture> m_texture;
    shared_ptr<gui::FontFace> m_font;

public:
    TextRenderer(const char *fontPath);

    void drawText(const char *text);
};

} }