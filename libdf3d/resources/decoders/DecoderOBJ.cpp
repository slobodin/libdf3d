#include "df3d_pch.h"
#include "DecoderOBJ.h"

#include <boost/algorithm/string.hpp>

#include <base/SystemsMacro.h>
#include <resources/FileDataSource.h>
#include <resources/FileSystem.h>
#include <resources/ResourceFactory.h>
#include <render/VertexIndexBuffer.h>
#include <render/Material.h>
#include <render/Technique.h>
#include <render/RenderPass.h>
#include <render/MeshData.h>
#include <render/Shader.h>
#include <render/GpuProgram.h>
#include <render/MaterialLib.h>
#include <utils/MeshIndexer.h>
#include <utils/Utils.h>

namespace df3d { namespace resources {

bool DecoderOBJ::hasNormals() const
{
    return m_normals.size() > 0;
}

bool DecoderOBJ::hasTxCoords() const
{
    return m_txCoords.size() > 0;
}

shared_ptr<render::VertexBuffer> DecoderOBJ::createVertexBuffer()
{
    auto vb = make_shared<render::VertexBuffer>(render::VertexFormat::create("p:3, n:3, tx:2, c:4, tan:3, bitan:3"));
    vb->setUsageType(render::GpuBufferUsageType::STATIC);

    return vb;
}

void DecoderOBJ::createDefaultMaterial(const std::string &filename)
{
    // Default material is used for vertex buffers without any material at all.
    m_defaultMaterial = std::make_shared<render::Material>(filename);
    render::Technique defaultTech(filename);
    render::RenderPass pass;

    defaultTech.appendPass(pass);

    m_defaultMaterial->appendTechnique(defaultTech);
    m_defaultMaterial->setCurrentTechnique(filename);
}

void DecoderOBJ::createMaterials(const std::string &dirPath, const std::string &filePath)
{
    // Try to open material library.
    auto fullp = FileSystem::pathConcatenate(dirPath, filePath);
    m_materials = g_resourceManager->getFactory().createMaterialLib(fullp);
}

void DecoderOBJ::processLine_v(std::istream &is)
{
    glm::vec3 v;
    is >> v.x >> v.y >> v.z;

    m_vertices.push_back(v);
}

void DecoderOBJ::processLine_vt(std::istream &is)
{
    glm::vec2 uv;
    is >> uv.x >> uv.y;

    // NOTE:
    // Invert for OpenGL
    // FIXME: if DirectX???
    uv.y = 1.0f - uv.y;

    m_txCoords.push_back(uv);
}

void DecoderOBJ::processLine_vn(std::istream &is)
{
    glm::vec3 vertexNormal;
    is >> vertexNormal.x >> vertexNormal.y >> vertexNormal.z;

    m_normals.push_back(vertexNormal);
}

void DecoderOBJ::processLine_vp(std::istream &is)
{
    utils::skip_line(is);
}

void DecoderOBJ::processLine_f(std::istream &is)
{
    // If do not have vertex buffer yet, then create default and set is as current.
    if (!m_currentVb)
    {
        m_vbWithDefaultMaterial = createVertexBuffer();
        m_currentVb = m_vbWithDefaultMaterial;
    }

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

        m_currentVb->appendVertexData((const float *)&v, 1);

        if (!is.good())
            break;
    }
}

void DecoderOBJ::processLine_mtl(std::istream &is)
{
    std::string material;
    is >> material;

    if (m_vertexBuffers.find(material) == m_vertexBuffers.end())
    {
        m_vertexBuffers[material] = createVertexBuffer();
        m_currentVb = m_vertexBuffers[material];
    }
    else
    {
        m_currentVb = m_vertexBuffers[material];
    }
}

void DecoderOBJ::processLine_o(std::istream &is)
{
    utils::skip_line(is);
}

void DecoderOBJ::processLine_g(std::istream &is)
{
    utils::skip_line(is);
}

void DecoderOBJ::processLine_s(std::istream &is)
{
    utils::skip_line(is);
}

DecoderOBJ::DecoderOBJ()
{
}

DecoderOBJ::~DecoderOBJ()
{
}

shared_ptr<Resource> DecoderOBJ::createResource()
{
    return make_shared<render::MeshData>();
}

bool DecoderOBJ::decodeResource(shared_ptr<FileDataSource> file, shared_ptr<Resource> resource)
{
    if (!file || !file->valid())
    {
        base::glog << "Can not decode resource. File source is invalid!" << base::logwarn;
        return false;
    }

    auto mesh = dynamic_pointer_cast<render::MeshData>(resource);
    if (!mesh)
        return false;

    // Parse obj.
    std::string buffer(file->getSize(), 0);
    file->getRaw(&buffer[0], file->getSize());

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
            auto dir = FileSystem::getFileDirectory(file->getPath());
            std::string fileName;
            input >> fileName;

            createMaterials(dir, fileName);
        }
    }

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

    return true;
}

} }
