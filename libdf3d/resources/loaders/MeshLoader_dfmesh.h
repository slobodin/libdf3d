#pragma once

#include "MeshLoaders.h"

namespace df3d { namespace resources {

//! DFMesh file format decoder.
/*!
 * DFMesh is a file format for meshes used in libdf3d.
 * Basically it contains a number of submesh vertex/index data with corresponding material.
 * It may include convex hull for physics support.
 * TODO: more features.
 */
class MeshLoader_dfmesh
{
public:
    unique_ptr<MeshDataFSLoader::Mesh> load(shared_ptr<FileDataSource> source);
};

} }
