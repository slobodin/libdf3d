#pragma once

#include "MeshLoaders.h"

namespace df3d { namespace resources {

class MeshLoader_obj
{
public:
    std::unique_ptr<MeshDataFSLoader::Mesh> load(shared_ptr<FileDataSource> source);
};

} }
