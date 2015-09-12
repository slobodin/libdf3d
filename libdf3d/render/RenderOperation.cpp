#include "df3d_pch.h"
#include "RenderOperation.h"

#include "VertexIndexBuffer.h"

namespace df3d { namespace render {

bool RenderOperation::isEmpty() const
{
    return vertexData ? vertexData->getVerticesUsed() == 0 : true;
}

} }
