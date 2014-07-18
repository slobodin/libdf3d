#include "df3d_pch.h"
#include "DecoderTTF.h"

#include <resources/FileDataSource.h>
#include <gui/FontFace.h>

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

    fontFace->m_font = TTF_OpenFontRW(file->getSdlRwops(), 0, 18);
    if (!fontFace->m_font)
    {
        base::glog << "Can't load ttf font from" << file->getPath() << base::logwarn;
        base::glog << TTF_GetError() << base::logwarn;
        return false;
    }

    return true;
}

} }