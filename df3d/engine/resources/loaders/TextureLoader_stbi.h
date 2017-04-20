#pragma once

namespace df3d {

class ResourceDataSource;
struct TextureResourceData;
class Allocator;

TextureResourceData* TextureLoader_stbi(ResourceDataSource &dataSource, Allocator &alloc, bool forceRGBA);

}
