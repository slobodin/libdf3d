#include "FrameStats.h"

#include <libdf3d/render/Texture.h>

namespace df3d {

void FrameStats::addTexture(const Texture &tex)
{
    m_textures++;

    // TODO_render
    //m_textureMemoryBytes += tex.getSizeInBytes();
}

void FrameStats::removeTexture(const Texture &tex)
{
    assert(m_textures > 0);
    m_textures--;

    // TODO_render
    //m_textureMemoryBytes -= tex.getSizeInBytes();
}
/*
void FrameStats::addRenderOperation(VertexBuffer *vb, IndexBuffer *ib, RenderOperation::Type type)
{
    m_drawCalls++;

    switch (type)
    {
    case RenderOperation::Type::LINES:
        if (ib)
            m_totalLines +=ib->getIndicesUsed() / 2;
        else
            m_totalLines += vb->getVerticesUsed() / 2;
        break;
    case RenderOperation::Type::TRIANGLES:
        if (ib)
            m_totalTriangles += ib->getIndicesUsed() / 3;
        else
            m_totalTriangles += vb->getVerticesUsed() / 3;
        break;
    default:
        break;
    }
}
*/
void FrameStats::addVertexBuffer()
{

}

void FrameStats::addIndexBuffer()
{

}

void FrameStats::addLight()
{
    m_totalLights++;
}

void FrameStats::reset()
{
    m_drawCalls = 0;
    m_totalTriangles = 0;
    m_totalLines = 0;
}

}
