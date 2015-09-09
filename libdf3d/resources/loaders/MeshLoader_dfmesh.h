#pragma once

#include "MeshLoaders.h"

namespace df3d { namespace resources {

class MeshLoader_dfmesh
{
public:
    std::unique_ptr<MeshDataFSLoader::Mesh> load(shared_ptr<FileDataSource> source);
};

} }
