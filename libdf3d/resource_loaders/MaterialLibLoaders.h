#pragma once

#include <resources/Resource.h>
#include <render/MaterialLib.h>

namespace df3d {

class MaterialLibFSLoader : public FSResourceLoader
{
    std::string m_path;
    std::vector<shared_ptr<Material>> m_materials;

public:
    MaterialLibFSLoader(const std::string &path);

    MaterialLib* createDummy() override;
    bool decode(shared_ptr<FileDataSource> source) override;
    void onDecoded(Resource *resource) override;
};

}