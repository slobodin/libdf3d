#pragma once

#include <df3d/lib/containers/PodArray.h>
#include <df3d/engine/render/RenderCommon.h>

namespace df3d {

struct Vertex_p3_n3_tx2_tan_bitan;

class DF3D_DLL MeshUtils
{
public:
    static void indexize(Vertex_p3_n3_tx2_tan_bitan *vdata, size_t count,
                         PodArray<Vertex_p3_n3_tx2_tan_bitan> &outVertices, PodArray<uint8_t> &outIndices,
                         IndicesType &outIndicesType);
    static void computeTangentBasis(Vertex_p3_n3_tx2_tan_bitan *vdata, size_t count);
};

}
