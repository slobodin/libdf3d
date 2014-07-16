#include "df3d_pch.h"
#include "DecoderTTF.h"

#include <resources/FileDataSource.h>
#include <gui/FontFace.h>

static FT_Library ft = nullptr;

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
    
    if (!ft)
    {
        if (FT_Init_FreeType(&ft))
        {
            base::glog << "Failed to init freetype library." << base::logwarn;
            return false;
        }
    }

    FT_Face face;
    boost::scoped_array<FT_Byte> buffer(new FT_Byte[file->getSize()]);
    file->getRaw(&buffer[0], file->getSize());

    if (FT_New_Memory_Face(ft, buffer.get(), file->getSize(), 0, &face))
    {
        base::glog << "Couldn't open font from" << file->getPath() << base::logwarn;
        return false;
    }

    fontFace->setFtFace(face);

    // FIXME:
    //FT_Done_FreeType(ft);

    return true;
}

} }