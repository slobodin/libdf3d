#include "df3d_pch.h"
#include "DecoderTerrain.h"

namespace df3d { namespace resources {

bool DecoderTerrain::doTerrainCreate(shared_ptr<render::MeshData> mesh)
{
    // Load height map.
    //auto heightMap = g_resourceManager->getResource<render::Texture>(m_heightMapPath, resources::ResourceManager::LOAD_MODE_IMMEDIATE);
    //if (!heightMap)
    //    return false;

    //// Height map dimensions.
    //size_t columns = heightMap->width();
    //size_t rows = heightMap->height();

    //// Create vertex buffer.
    //auto vb = make_shared<render::VertexBuffer>();
    //vb->setType(render::VertexBuffer::INDEXED_TRIANGLE_LIST);

    // Prepare material.
    // TODO:
    assert(false);
    /*auto material = make_shared<render::Material>();;
    if (m_materialLib == "" && m_submaterialName == "")
        material->setGpuProgram(g_renderManager->getRenderer()->createFFPGpuProgram());
    else
        material->initFromFile(m_materialLib, m_submaterialName);

    // FIXME:
    // Got only first texture...
    auto texture = material->getSamplers().size() ? material->getSamplers()[0].texture : nullptr;

    float colStepText = 0.0f, rowStepText = 0.0f;
    if (texture)
    {
        colStepText = ((float)texture->width() - 1.0f) / (columns - 1.0f);
        rowStepText = ((float)texture->height() - 1.0f) / (rows - 1.0f);
    }

    // Create vertices.
    render::VertexBuffer::VertexArray &vertices = vb->getVertices();
    vertices.reserve(rows * columns);

    auto terrainWidth = 1.0f;
    auto terrainHeight = 1.0f;

    float colStep = terrainWidth / (float)(columns - 1);
    float rowStep = terrainHeight / (float)(rows - 1);

    for (size_t currRow = 0; currRow < rows; currRow++)
    {
        for (size_t currCol = 0; currCol < columns; currCol++)
        {
            render::Vertex v;

            v.p.x = currCol * colStep - (terrainWidth / 2.0f);      // (0;0) point is the center of the map
            v.p.z = currRow * rowStep - (terrainHeight / 2.0f);

            auto pixelStart = heightMap->pixelAt(currCol, currRow);

            v.p.y = ((float)(*pixelStart)) / 255.0f;

            if (texture)
            {
                v.t.x = currCol * colStepText;
                v.t.y = currRow * rowStepText;

                v.t.x /= float(texture->width() - 1.0f);   // normalize
                v.t.y /= float(texture->height() - 1.0f);   // normalize
            }

            vertices.push_back(v);
        }
    }

    // Create triangles.
    render::VertexBuffer::IndexArray &indices = vb->getIndices();
    for (size_t poly = 0; poly < ((columns - 1) * (rows - 1)); poly++)
    {
        int basePolyInd = (poly % (columns - 1)) + (columns * (poly / (columns - 1)));

        indices.push_back(basePolyInd);
        indices.push_back(basePolyInd + columns);
        indices.push_back(basePolyInd + columns + 1);

        indices.push_back(basePolyInd);
        indices.push_back(basePolyInd + columns + 1);
        indices.push_back(basePolyInd + 1);
    }

    vb->computeNormals();
    vb->computeTangentBasis();

    mesh->attachSubMesh(vb, material);

    // No more need height map.
    g_resourceManager->unloadResource(heightMap);*/

    return true;
}

DecoderTerrain::DecoderTerrain()
{
}

DecoderTerrain::~DecoderTerrain()
{
}

shared_ptr<Resource> DecoderTerrain::createResource()
{
    return nullptr;
    //return make_shared<render::MeshData>();
}

bool DecoderTerrain::decodeResource(shared_ptr<FileDataSource> file, shared_ptr<Resource> resource)
{
    // TODO:
    return false;
    //if (!file || !file->valid())
    //    return false;

    //auto mesh = boost::dynamic_pointer_cast<render::MeshData>(resource);
    //if (!mesh)
    //    return false;

    //std::string buffer(file->getSize(), 0);
    //file->getRaw(&buffer[0], file->getSize());

    //std::istringstream input(std::move(buffer));
    //std::string prefix;
    //while (input >> prefix)
    //{
    //    if (prefix == "heightMap")
    //        input >> m_heightMapPath;
    //    else if (prefix == "materiallib")
    //        input >> m_materialLib;
    //    else if (prefix == "submaterial")
    //        input >> m_submaterialName;
    //}

    //return doTerrainCreate(mesh);
}

} }