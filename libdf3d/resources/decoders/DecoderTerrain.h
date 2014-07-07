#pragma once

#include "../ResourceDecoder.h"

FWD_MODULE_CLASS(render, MeshData)

namespace df3d { namespace resources {

class DecoderTerrain : public ResourceDecoder
{
    std::string m_heightMapPath;
    std::string m_materialLib;
    std::string m_submaterialName;

    bool doTerrainCreate(shared_ptr<render::MeshData> mesh);

public:
    DecoderTerrain();
    ~DecoderTerrain();

    shared_ptr<Resource> createResource();
    bool decodeResource(const shared_ptr<FileDataSource> file, shared_ptr<Resource> resource);
};

} }