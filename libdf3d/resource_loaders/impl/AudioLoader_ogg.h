#pragma once

#include "../AudioLoaders.h"

namespace df3d { namespace resource_loaders_impl {

class AudioLoader_ogg
{
public:
    unique_ptr<AudioBufferFSLoader::PCMData> load(shared_ptr<FileDataSource> source);
};

} }

