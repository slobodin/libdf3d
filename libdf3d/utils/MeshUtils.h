#pragma once

FWD_MODULE_CLASS(render, SubMesh)

namespace df3d { namespace utils { namespace mesh {

void DF3D_DLL indexize();
void DF3D_DLL computeNormals(render::SubMesh &submesh);
void DF3D_DLL computeTangentBasis(render::SubMesh &submesh);

} } }
