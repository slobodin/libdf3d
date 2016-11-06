#pragma once

namespace df3d {

struct PCMData;
class Allocator;

unique_ptr<PCMData> AudioLoader_wav(const char *path, Allocator &alloc);

}
