#include "MeshLoader_obj.h"

#include <df3d/engine/render/MeshUtils.h>
#include <df3d/engine/EngineCVars.h>
#include <df3d/engine/resources/MeshResource.h>
#include <df3d/engine/resources/ResourceDataSource.h>
#include <df3d/lib/Utils.h>

namespace df3d {

class MeshLoader_obj_state
{
    Allocator &m_alloc;

    PodArray<glm::vec3> m_vertices;
    PodArray<glm::vec3> m_normals;
    PodArray<glm::vec2> m_txCoords;

    // Merged (by material) submeshes.
    std::unordered_map<std::string, MeshResourceData::Part*> m_meshParts;
    MeshResourceData::Part *m_currentMeshPart = nullptr;
    MeshResourceData::Part* createMeshPart(const std::string &materialName)
    {
        const auto &vertexFormat = Vertex_p_n_tx_tan_bitan::getFormat();
        return MAKE_NEW(m_alloc, MeshResourceData::Part)(vertexFormat, m_alloc);
    }

    bool hasNormals() const { return m_normals.size() > 0; }
    bool hasTxCoords() const { return m_txCoords.size() > 0; }

    void processLine_v(std::istream &is)
    {
        glm::vec3 v;
        is >> v.x >> v.y >> v.z;

        m_vertices.push_back(v);
    }

    void processLine_vt(std::istream &is)
    {
        glm::vec2 uv;
        is >> uv.x >> uv.y;

        // NOTE:
        // Invert for OpenGL
        // FIXME: if DirectX???
        uv.y = 1.0f - uv.y;

        m_txCoords.push_back(uv);
    }

    void processLine_vn(std::istream &is)
    {
        glm::vec3 vertexNormal;
        is >> vertexNormal.x >> vertexNormal.y >> vertexNormal.z;

        m_normals.push_back(vertexNormal);
    }

    void processLine_vp(std::istream &is)
    {
        utils::skip_line(is);
    }

    void processLine_f(std::istream &is)
    {
        DF3D_ASSERT(m_currentMeshPart != nullptr);

        size_t verticesCount = 0;
        // FIXME:
        // Only triangles.
        while (verticesCount < 3)
        {
            char temp;

            verticesCount++;
            int vertexidx = 0;
            int normalidx = 0;
            int uvidx = 0;

            // vertex
            if (!hasNormals() && !hasTxCoords())
            {
                is >> vertexidx;
                DF3D_ASSERT(vertexidx >= 1);
            }
            // vertex//normal
            else if (!hasTxCoords())
            {
                is >> vertexidx >> temp >> temp >> normalidx;
                DF3D_ASSERT(vertexidx >= 1 && normalidx >= 1);
            }
            // vertex/texture
            else if (!hasNormals())
            {
                is >> vertexidx >> temp >> uvidx;
                DF3D_ASSERT(vertexidx >= 1 && uvidx >= 1);
            }
            // vertex/texture/normal
            else
            {
                is >> vertexidx >> temp >> uvidx >> temp >> normalidx;
                DF3D_ASSERT(vertexidx >= 1 && uvidx >= 1 && normalidx >= 1);
            }

            auto &vdata = m_currentMeshPart->vertexData;
            vdata.addVertex();

            auto v = (Vertex_p_n_tx_tan_bitan *)vdata.getVertex(vdata.getVerticesCount() - 1);

            v->pos = m_vertices[vertexidx - 1];
            if (normalidx > 0)
                v->normal = m_normals[normalidx - 1];
            else
                v->normal = { 0.0f, 0.0f, 0.0f };
            if (uvidx > 0)
                v->uv = m_txCoords[uvidx - 1];
            else
                v->uv = { 0.0f, 0.0f };

            v->tangent = {};
            v->bitangent = {};

            if (!is.good())
                break;
        }

        DF3D_ASSERT_MESS(verticesCount == 3, "Only triangles supported in obj loader");
    }

    void processLine_mtl(std::istream &is)
    {
        std::string material;
        is >> material;

        // Create new vertex cache if not found, set as current.
        auto found = m_meshParts.find(material);
        if (found == m_meshParts.end())
        {
            auto meshPart = createMeshPart(material);

            m_meshParts[material] = meshPart;
            m_currentMeshPart = meshPart;
        }
        else
        {
            m_currentMeshPart = found->second;
        }
    }

    void processLine_o(std::istream &is)
    {
        utils::skip_line(is);
    }

    void processLine_g(std::istream &is)
    {
        utils::skip_line(is);
    }

    void processLine_s(std::istream &is)
    {
        utils::skip_line(is);
    }

public:
    MeshLoader_obj_state(Allocator &alloc)
        : m_alloc(alloc),
        m_vertices(alloc),
        m_normals(alloc),
        m_txCoords(alloc)
    {

    }

    ~MeshLoader_obj_state()
    {

    }

    MeshResourceData* load(ResourceDataSource &dataSource)
    {
        // Parse obj. TODO: can use stream directly.
        std::string buffer(dataSource.getSize(), 0);
        dataSource.read(&buffer[0], buffer.size());

        std::istringstream input(std::move(buffer));
        std::string tok;
        while (input >> tok)
        {
            utils::trim_left(tok);

            if (tok.empty() || tok[0] == '#')
            {
                utils::skip_line(input);
                continue;
            }

            if (tok == "f")
                processLine_f(input);
            else if (tok == "v")
                processLine_v(input);
            else if (tok == "vt")
                processLine_vt(input);
            else if (tok == "vn")
                processLine_vn(input);
            else if (tok == "vp")
                processLine_vp(input);
            else if (tok == "usemtl")
                processLine_mtl(input);
            else if (tok == "o")
                processLine_o(input);
            else if (tok == "g")
                processLine_g(input);
            else if (tok == "s")
                processLine_s(input);
            //else if (tok == "mtllib") {}
        }

        bool computeNormals = !hasNormals();

        auto result = MAKE_NEW(m_alloc, MeshResourceData)();

        for (auto &kv : m_meshParts)
        {
            if (computeNormals)
            {
                //DFLOG_DEBUG("Computing normals in %s", dataSource.getResourceId().c_str());
                DF3D_ASSERT(false);
                //MeshUtils::computeNormals(*s.second);
            }

            auto meshPart = kv.second;

            const auto vData = (Vertex_p_n_tx_tan_bitan*)meshPart->vertexData.getRawData();
            const auto vCount = meshPart->vertexData.getVerticesCount();
            MeshUtils::computeTangentBasis(vData, vCount);

            if (EngineCVars::objIndexize)
            {
                PodArray<Vertex_p_n_tx_tan_bitan> indexedVertices(m_alloc);
                PodArray<uint32_t> indices(m_alloc);

                MeshUtils::indexize(vData, vCount, indexedVertices, indices);

                /*
                DFLOG_DEBUG("Vertices before: %d, AFTER indexed: %d. Indices %d", vCount, indexedVertices.size(), indices.size());
                DFLOG_DEBUG("Size before %d KB, size after %d KB", utils::sizeKB(sizeof(Vertex_p_n_tx_tan_bitan) * vCount),
                utils::sizeKB(sizeof(Vertex_p_n_tx_tan_bitan) * indexedVertices.size() + indices.size() * sizeof(uint32_t)));
                */

                VertexData newData(meshPart->vertexData.getFormat());
                newData.addVertices(indexedVertices.size());
                memcpy(newData.getRawData(), indexedVertices.data(), newData.getSizeInBytes());

                meshPart->vertexData = std::move(newData);
                meshPart->indices = std::move(indices);
            }

            meshPart->materialName = kv.first;
            result->parts.push_back(meshPart);
        }

        return result;
    }
};

MeshResourceData* MeshLoader_obj(ResourceDataSource &dataSource, Allocator &alloc)
{
    auto loader = MAKE_NEW(alloc, MeshLoader_obj_state)(alloc);
    auto result = loader->load(dataSource);
    MAKE_DELETE(alloc, loader);
    return result;
}

}
