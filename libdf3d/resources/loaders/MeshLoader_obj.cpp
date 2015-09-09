#include "df3d_pch.h"
#include "MeshLoader_obj.h"

#include <boost/algorithm/string.hpp>

#include <base/SystemsMacro.h>
#include <resources/FileSystem.h>
#include <resources/FileDataSource.h>
#include <resources/ResourceFactory.h>
#include <utils/Utils.h>

namespace df3d { namespace resources {

bool MeshLoader_obj::hasNormals() const
{
    return m_normals.size() > 0;
}

bool MeshLoader_obj::hasTxCoords() const
{
    return m_txCoords.size() > 0;
}

shared_ptr<render::SubMesh> MeshLoader_obj::createSubmesh(const std::string &materialName)
{
    auto submesh = make_shared<render::SubMesh>();
    submesh->setVertexFormat(render::VertexFormat::create("p:3, n:3, tx:2, c:4, tan:3, bitan:3"));
    submesh->setMtlName(materialName);

    // TODO_REFACTO:
    // usage type? STATIC

    return submesh;
}

//void MeshLoader_obj::createDefaultMaterial(const std::string &filename)
//{
//    // Default material is used for vertex buffers without any material at all.
//    m_defaultMaterial = std::make_unique<render::Material>(filename);
//    render::Technique defaultTech(filename);
//    render::RenderPass pass;
//
//    defaultTech.appendPass(pass);
//
//    m_defaultMaterial->appendTechnique(defaultTech);
//    m_defaultMaterial->setCurrentTechnique(filename);
//}
//
//void MeshLoader_obj::createMaterials(const std::string &dirPath, const std::string &filePath)
//{
//    // Try to open material library.
//    auto fullp = FileSystem::pathConcatenate(dirPath, filePath);
//    m_materials = g_resourceManager->getFactory().createMaterialLib(fullp);
//}

void MeshLoader_obj::processLine_v(std::istream &is)
{
    glm::vec3 v;
    is >> v.x >> v.y >> v.z;

    m_vertices.push_back(v);
}

void MeshLoader_obj::processLine_vt(std::istream &is)
{
    glm::vec2 uv;
    is >> uv.x >> uv.y;

    // NOTE:
    // Invert for OpenGL
    // FIXME: if DirectX???
    uv.y = 1.0f - uv.y;

    m_txCoords.push_back(uv);
}

void MeshLoader_obj::processLine_vn(std::istream &is)
{
    glm::vec3 vertexNormal;
    is >> vertexNormal.x >> vertexNormal.y >> vertexNormal.z;

    m_normals.push_back(vertexNormal);
}

void MeshLoader_obj::processLine_vp(std::istream &is)
{
    utils::skip_line(is);
}

void MeshLoader_obj::processLine_f(std::istream &is)
{
    // If do not have submesh yet, then create default and set is as current.
    if (!m_currentSubmesh)
        m_currentSubmesh = createSubmesh(m_meshDataFileName);   // Using filename as the material name.

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
        }
        // vertex//normal
        else if (!hasTxCoords())
        {
            is >> vertexidx >> temp >> temp >> normalidx;
        }
        // vertex/texture
        else if (!hasNormals())
        {
            is >> vertexidx >> temp >> uvidx;
        }
        // vertex/texture/normal
        else
        {
            is >> vertexidx >> temp >> uvidx >> temp >> normalidx;
        }

        render::Vertex_3p3n2tx4c3t3b v;

        v.p = m_vertices.at(vertexidx - 1);

        if (normalidx > 0)
            v.n = m_normals.at(normalidx - 1);
        if (uvidx > 0)
            v.tx = m_txCoords.at(uvidx - 1);

        m_currentSubmesh->appendVertexData((const float *)&v, 1);

        if (!is.good())
            break;
    }
}

void MeshLoader_obj::processLine_mtl(std::istream &is)
{
    std::string material;
    is >> material;

    // Create new vertex cache if not found, set as current.
    auto found = m_submeshes.find(material);
    if (found == m_submeshes.end())
    {
        m_currentSubmesh = createSubmesh(material);
        m_submeshes[material] = m_currentSubmesh;
    }
    else
    {
        m_currentSubmesh = found->second;
    }
}

void MeshLoader_obj::processLine_o(std::istream &is)
{
    utils::skip_line(is);
}

void MeshLoader_obj::processLine_g(std::istream &is)
{
    utils::skip_line(is);
}

void MeshLoader_obj::processLine_s(std::istream &is)
{
    utils::skip_line(is);
}

std::unique_ptr<MeshDataFSLoader::Mesh> MeshLoader_obj::load(shared_ptr<FileDataSource> source)
{
    m_meshDataFileName = source->getPath();

    // Parse obj. TODO: can use stream directly.
    std::string buffer(source->getSize(), 0);
    source->getRaw(&buffer[0], source->getSize());

    std::istringstream input(std::move(buffer));
    std::string tok;
    while (input >> tok)
    {
        boost::trim_left(tok);

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
        else if (tok == "mtllib")
        {
            auto dir = FileSystem::getFileDirectory(source->getPath());
            std::string fileName;
            input >> fileName;

            m_materialLibPath = g_fileSystem->fullPath(FileSystem::pathConcatenate(dir, fileName));
        }
    }

    // Set material lib path for all the submeshes.
    for (auto submesh : m_submeshes)
        submesh.second->setMtlLibPath(m_materialLibPath);

    bool computeNormals = m_normals.size() > 0 ? false : true;

    // TODO_REFACTO

    assert(false);

    /*

    // Post init.

    // Attaches submeshes to the mesh and sets up materials.
    auto initVb = [&](shared_ptr<render::VertexBuffer> vb, shared_ptr<render::Material> m)
    {
    auto submesh = make_shared<render::SubMesh>();
    submesh->setMaterial(m);
    submesh->setVertexBuffer(vb);
    if (computeNormals)
    submesh->computeNormals();

    submesh->computeTangentBasis();

    // Indexize vertex buffer.
    utils::MeshIndexer indexer;
    //indexer.index(vb);

    mesh->attachSubMesh(submesh);
    };

    // Attach submeshes with given materials.
    for (auto vb : m_vertexBuffers)
    {
    // If m_materials someway became empty (if mtl lib wasn't found), then use default material.
    if (!m_materials->isMaterialExists(vb.first))
    initVb(vb.second, m_defaultMaterial);
    else
    initVb(vb.second, m_materials->getMaterial(vb.first));
    }

    // Attach submeshes without OBJ material (use default instead).
    if (m_vbWithDefaultMaterial)
    {
    createDefaultMaterial(file->getPath());
    initVb(m_vbWithDefaultMaterial, m_defaultMaterial);
    }

    */

    // TODO: can also unload materiallib from resource manager.

    // Clear all the state.
    m_materialLibPath.clear();
    m_meshDataFileName.clear();

    return nullptr;
}

} }
