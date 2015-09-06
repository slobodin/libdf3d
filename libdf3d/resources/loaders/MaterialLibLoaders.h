#pragma once

#include <resources/Resource.h>
#include <render/MaterialLib.h>

namespace df3d { namespace resources {

class MaterialLibFSLoader : public FSResourceLoader
{
    std::string m_path;
    std::vector<render::Material> m_materials;

public:
    MaterialLibFSLoader(const std::string &path);

    render::MaterialLib* createDummy(const ResourceGUID &guid) override;
    void decode(shared_ptr<FileDataSource> source) override;
    void onDecoded(Resource *resource) override;
};

} }
