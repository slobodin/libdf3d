#pragma once

#include "MeshLoaders.h"

namespace df3d {

class MaterialLib;

class MeshLoader_obj
{
    std::vector<glm::vec3> m_vertices;
    std::vector<glm::vec3> m_normals;
    std::vector<glm::vec2> m_txCoords;

    bool hasNormals() const;
    bool hasTxCoords() const;

    // Merged (by material) submeshes.
    std::map<std::string, unique_ptr<SubMesh>> m_submeshes;

    SubMesh* m_currentSubmesh;
    std::unordered_map<SubMesh*, std::string> m_materialNameLookup;
    unique_ptr<SubMesh> createSubmesh(const std::string &materialName);

    void processLine_v(std::istream &is);
    void processLine_vt(std::istream &is);
    void processLine_vn(std::istream &is);
    void processLine_vp(std::istream &is);
    void processLine_f(std::istream &is);
    void processLine_mtl(std::istream &is);
    void processLine_o(std::istream &is);
    void processLine_g(std::istream &is);
    void processLine_s(std::istream &is);

    std::string m_materialLibPath;
    std::string m_meshDataFileName;

public:
    unique_ptr<MeshDataFSLoader::Mesh> load(shared_ptr<FileDataSource> source);
};

}
