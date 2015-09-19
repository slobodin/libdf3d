#pragma once

FWD_MODULE_CLASS(render, SubMesh)

namespace df3d { namespace utils { namespace mesh {

DF3D_DLL void indexize();
DF3D_DLL void computeNormals(render::SubMesh &submesh);
DF3D_DLL void computeTangentBasis(render::SubMesh &submesh);

} } }
