#include "df3d_pch.h"
#include "ResourceDecoder.h"

#include <base/SystemsMacro.h>
#include "decoders/DecoderOBJ.h"
#include "decoders/DecoderMTL.h"
#include "decoders/DecoderTexture.h"
#include "decoders/DecoderTerrain.h"
#include "decoders/DecoderWAV.h"
#include "decoders/DecoderOGG.h"

namespace df3d { namespace resources {

ResourceDecoder::ResourceDecoder()
{
}

ResourceDecoder::~ResourceDecoder()
{
}

shared_ptr<ResourceDecoder> createResourceDecoder(const std::string &filepath)
{
    auto extension = g_fileSystem->getFileExtension(filepath);
    if (extension == ".obj")
        return make_shared<DecoderOBJ>();
    if (extension == ".mtl")
        return make_shared<DecoderMTL>();
    if (extension == ".png" || extension == ".jpg" || extension == ".jpeg" || extension == ".bmp")
        return nullptr;
    if (extension == ".wav")
        return make_shared<DecoderWAV>();
    if (extension == ".ogg")
        return make_shared<DecoderOGG>();
    if (extension == ".terrain")
        return make_shared<DecoderTerrain>();
    else
    {
        base::glog << "Decoder for resources of type" << extension << "doesn't exist." << base::logwarn;
        return nullptr;
    }
}

} }
