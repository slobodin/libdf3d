#include "df3d_pch.h"
#include "RenderOperation.h"

#include "VertexIndexBuffer.h"

namespace df3d { namespace render {

RenderOperation::RenderOperation()
{
}

RenderOperation::~RenderOperation()
{
}

bool RenderOperation::isEmpty() const
{
    return vertexData ? vertexData->getElementsUsed() == 0 : true;
}

} }
