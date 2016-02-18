#pragma once

#include <libdf3d/render/RenderOperation.h>

namespace df3d {

class Texture;

class DF3D_DLL FrameStats
{
    size_t m_drawCalls = 0;
    size_t m_totalTriangles = 0;
    size_t m_totalLines = 0;

    size_t m_textures = 0;
    size_t m_textureMemoryBytes = 0;

    size_t m_vertexDataBytes = 0;
    size_t m_indexDataBytes = 0;

    size_t m_totalLights = 0;

public:
    void addTexture(const Texture &tex);
    void removeTexture(const Texture &tex);

    void addRenderOperation(VertexBuffer *vb, IndexBuffer *ib, RenderOperation::Type type);
    void addVertexBuffer();
    void addIndexBuffer();

    void addLight();

    void reset();

    size_t getDrawCallsCount() const { return m_drawCalls; }
    size_t getTrianglesCount() const { return m_totalTriangles; }
    size_t getLinesCount() const { return m_totalLines; }
    size_t getTexturesCount() const { return m_textures; }
    size_t getTextureDataBytes() const { return m_textureMemoryBytes; }
    size_t getVertexDataBytes() const { return m_vertexDataBytes; }
    size_t getIndexDataBytes() const { return m_indexDataBytes; }
    size_t getLightsCount() const { return m_totalLights; }
};

}
