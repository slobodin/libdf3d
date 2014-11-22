#pragma once

#include "../ResourceDecoder.h"
#include <render/Vertex.h>

FWD_MODULE_CLASS(render, VertexBuffer)
FWD_MODULE_CLASS(render, Material)
FWD_MODULE_CLASS(render, MaterialLib)

namespace df3d { namespace resources {

/**
  * Wavefront OBJ file decoder.
  * Supports only triangulated models with precomputed normals.
  */
class DecoderOBJ : public ResourceDecoder
{
    std::vector<glm::vec3> m_vertices;
    std::vector<glm::vec3> m_normals;
    std::vector<glm::vec2> m_txCoords;

    bool hasNormals() const;
    bool hasTxCoords() const;

    // If submesh doesn't have material then default will be set.
    shared_ptr<render::Material> m_defaultMaterial;
    // Material library.
    shared_ptr<render::MaterialLib> m_materials;
    // Merged (by material) vertex buffers.
    std::map<std::string, shared_ptr<render::VertexBuffer>> m_vertexBuffers;
    shared_ptr<render::VertexBuffer> m_vbWithDefaultMaterial;

    shared_ptr<render::VertexBuffer> m_currentVb;
    shared_ptr<render::VertexBuffer> createVertexBuffer();

    void createDefaultMaterial(const std::string &filename);
    void createMaterials(const char *dirPath, const char *filePath);

    void processLine_v(std::istream &is);
    void processLine_vt(std::istream &is);
    void processLine_vn(std::istream &is);
    void processLine_vp(std::istream &is);
    void processLine_f(std::istream &is);
    void processLine_mtl(std::istream &is);
    void processLine_o(std::istream &is);
    void processLine_g(std::istream &is);
    void processLine_s(std::istream &is);

public:
    DecoderOBJ();
    ~DecoderOBJ();

    shared_ptr<Resource> createResource() override;
    bool decodeResource(const shared_ptr<FileDataSource> file, shared_ptr<Resource> resource) override;
};

} }