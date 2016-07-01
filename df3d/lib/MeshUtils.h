#pragma once

namespace df3d {

class SubMesh;

class DF3D_DLL MeshUtils
{
public:
    static void indexize();
    static void computeNormals(SubMesh &submesh);
    static void computeTangentBasis(SubMesh &submesh);
};

}
