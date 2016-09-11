#pragma once

#include <df3d/lib/containers/PodArray.h>

namespace df3d {

struct Vertex_p_n_tx_tan_bitan;

class DF3D_DLL MeshUtils
{
public:
    static void indexize(Vertex_p_n_tx_tan_bitan *vdata, size_t count,
                         PodArray<Vertex_p_n_tx_tan_bitan> &outVertices, PodArray<uint32_t> &outIndices);

    static void computeTangentBasis(Vertex_p_n_tx_tan_bitan *vdata, size_t count);
};

}
