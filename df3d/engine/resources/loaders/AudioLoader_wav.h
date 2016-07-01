#pragma once

#include "../AudioLoaders.h"

namespace df3d { namespace resource_loaders {

class AudioLoader_wav
{
public:
    unique_ptr<PCMData> load(shared_ptr<DataSource> source);
};

} }
