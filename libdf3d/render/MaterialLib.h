#pragma once

#include <libdf3d/resources/Resource.h>
#include "Material.h"

namespace df3d {

class MaterialLibFSLoader;

class DF3D_DLL MaterialLib : public Resource
{
    friend class MaterialLibFSLoader;

    std::unordered_map<std::string, shared_ptr<Material>> m_materials;

public:
    static std::vector<std::string> Defines;

    MaterialLib();
    ~MaterialLib();

    shared_ptr<Material> getMaterial(const std::string &name) const;

    void appendMaterial(shared_ptr<Material> material);

    bool isMaterialExists(const std::string &name);
    size_t materialsCount() const;
};

}
