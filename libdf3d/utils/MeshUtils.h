#pragma once

FWD_MODULE_CLASS(render, SubMesh)

namespace df3d { namespace utils { namespace mesh {

void indexize();
void computeNormals(render::SubMesh &submesh);
void computeTangentBasis(render::SubMesh &submesh);

} } }
