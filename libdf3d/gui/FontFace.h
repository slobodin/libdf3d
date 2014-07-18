#pragma once

#include <resources/Resource.h>

FWD_MODULE_CLASS(components, TextMeshComponent)
FWD_MODULE_CLASS(resources, DecoderTTF)
FWD_MODULE_CLASS(render, Image)

struct stbtt_fontinfo;

// FIXME:
// Using stb ttf instead SDL_ttf because of weird crashes during text rendering.
// Note: libRocket uses freetype as static library.
// Note: weird behavior when passing strings from python to drawText (TTF_RenderText_Solid gives 'Text has zero width'), SDL_stack_alloc???
// Note: crash when explicitly setting text string.

namespace df3d { namespace gui {

class FontFace : public resources::Resource
{
    friend class components::TextMeshComponent;
    friend class resources::DecoderTTF;

    scoped_ptr<stbtt_fontinfo> m_info;
    boost::scoped_array<unsigned char> m_data;

public:
    FontFace();
    ~FontFace();

    bool init();

    shared_ptr<render::Image> getGlyphImage(char c, int size);
};

} }