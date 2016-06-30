#pragma once

namespace df3d { class SubMesh; }

namespace df3d { namespace utils { namespace mesh {

DF3D_DLL void indexize();
DF3D_DLL void computeNormals(SubMesh &submesh);
DF3D_DLL void computeTangentBasis(SubMesh &submesh);

} } }
