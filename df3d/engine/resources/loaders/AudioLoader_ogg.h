#pragma once

#include "../AudioLoaders.h"

namespace df3d { namespace resource_loaders_impl {

class AudioLoader_ogg
{
public:
    unique_ptr<PCMData> load(shared_ptr<DataSource> source);
    unique_ptr<IAudioStream> loadStreamed(shared_ptr<DataSource> source);
};

} }

