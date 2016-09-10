#pragma once

namespace df3d {

struct Vertex_p3_n3_tx2_tan_bitan;

class DF3D_DLL MeshUtils
{
public:
    static void indexize();
    static void computeTangentBasis(Vertex_p3_n3_tx2_tan_bitan *vdata, size_t count);
};

}
