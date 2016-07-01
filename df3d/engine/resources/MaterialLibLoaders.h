#pragma once

#include <df3d/engine/resources/Resource.h>
#include <df3d/engine/render/MaterialLib.h>

namespace df3d {

class MaterialLibFSLoader : public FSResourceLoader
{
    std::string m_path;
    Json::Value m_root;

public:
    MaterialLibFSLoader(const std::string &path);

    MaterialLib* createDummy() override;
    bool decode(shared_ptr<DataSource> source) override;
    void onDecoded(Resource *resource) override;
};

}
