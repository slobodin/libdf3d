#include "df3d_pch.h"
#include "ResourceDecoder.h"

#include <base/SystemsMacro.h>
#include "decoders/DecoderOBJ.h"
#include "decoders/DecoderMTL.h"
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
    return nullptr;
}

} }
