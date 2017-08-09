#pragma once

#include <df3d/lib/containers/PodArray.h>

namespace df3d {

struct Vertex_p_n_tx_tan_bitan;

class MeshUtils
{
public:
    static void indexize(const Vertex_p_n_tx_tan_bitan *vdata, size_t count,
                         PodArray<Vertex_p_n_tx_tan_bitan> &outVertices, PodArray<uint16_t> &outIndices);

    static void computeTangentBasis(Vertex_p_n_tx_tan_bitan *vdata, size_t count);
    static void computeTangentBasis(Vertex_p_n_tx_tan_bitan *vdata, size_t verticesCount,
                                    const uint16_t *indices, size_t indicesCount);
};

}
