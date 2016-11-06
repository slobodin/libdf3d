#pragma once

namespace df3d {

struct PCMData;
class Allocator;
class IAudioStream;

unique_ptr<PCMData> AudioLoader_ogg(const char *path, Allocator &alloc);
unique_ptr<IAudioStream> AudioLoader_ogg_streamed(const char *path, Allocator &alloc);

}

