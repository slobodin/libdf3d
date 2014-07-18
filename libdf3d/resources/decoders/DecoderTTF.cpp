#include "df3d_pch.h"
#include "DecoderTTF.h"

#include <resources/FileDataSource.h>
#include <gui/FontFace.h>
#include <stb/stb_truetype.h>

namespace df3d { namespace resources {

DecoderTTF::DecoderTTF()
{
}

DecoderTTF::~DecoderTTF()
{
}

shared_ptr<Resource> DecoderTTF::createResource()
{
    return make_shared<gui::FontFace>();
}

bool DecoderTTF::decodeResource(const shared_ptr<FileDataSource> file, shared_ptr<Resource> resource)
{
    if (!file || !file->valid())
        return false;

    auto fontFace = boost::dynamic_pointer_cast<gui::FontFace>(resource);
    if (!fontFace)
        return false;

    fontFace->m_data.reset(new unsigned char[file->getSize()]);
    file->getRaw(&fontFace->m_data[0], file->getSize());

    auto offs = stbtt_GetFontOffsetForIndex(fontFace->m_data.get(), 0);
    auto res = stbtt_InitFont(fontFace->m_info.get(), fontFace->m_data.get(), offs);
    if (!res)
    {
        base::glog << "Failed to decode font from" << file->getPath() << base::logwarn;
        fontFace->m_data.reset();
        return false;
    }

    return true;
}

} }