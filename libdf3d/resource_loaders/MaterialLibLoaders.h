#pragma once

#include <libdf3d/resources/Resource.h>
#include <libdf3d/render/MaterialLib.h>

namespace df3d {

// TODO:
/*
class MaterialLibManualLoader : public ManualResourceLoader
{
    std::string m_materialData;

public:
    MaterialLibManualLoader(std::string &&materialData);

    MaterialLib* load() override;
};
*/

class MaterialLibFSLoader : public FSResourceLoader
{
    std::string m_path;
    Json::Value m_root;

public:
    MaterialLibFSLoader(const std::string &path);

    MaterialLib* createDummy() override;
    bool decode(shared_ptr<FileDataSource> source) override;
    void onDecoded(Resource *resource) override;
};

}
