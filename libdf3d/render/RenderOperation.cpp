#include "RenderOperation.h"

#include "VertexIndexBuffer.h"

namespace df3d {

bool RenderOperation::isEmpty() const
{
    return vertexData ? vertexData->getVerticesUsed() == 0 : true;
}

}
