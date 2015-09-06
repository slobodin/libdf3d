#pragma once

#include "resources/Resource.h"
#include "Material.h"

FWD_MODULE_CLASS(resources, MaterialLibFSLoader)

namespace df3d { namespace render {

class DF3D_DLL MaterialLib : public resources::Resource
{
    friend class resources::MaterialLibFSLoader;

    std::unordered_map<std::string, Material> m_materials;

public:
    static std::vector<std::string> Defines;

    MaterialLib();
    ~MaterialLib();

    const Material* getMaterial(const std::string &name) const;
    Material* getMaterial(const std::string &name);

    void appendMaterial(const Material &material);

    bool isMaterialExists(const std::string &name);
    size_t materialsCount() const;

    // FIXME:
    static Material* getMaterial(const std::string &mtlLibName, const std::string &mtlName);
};

} }
