#pragma once

#include "resources/Resource.h"

namespace df3d { namespace render {

class Material;

class DF3D_DLL MaterialLib : public resources::Resource
{
    std::unordered_map<std::string, shared_ptr<Material>> m_materials;
public:
    static std::vector<std::string> Defines;

    MaterialLib();
    ~MaterialLib();

    shared_ptr<Material> getMaterial(const std::string &name);
    void appendMaterial(shared_ptr<Material> material);

    bool isMaterialExists(const std::string &name);
    size_t materialCount() const;

    // FIXME:
    static shared_ptr<Material> getMaterial(const std::string &mtlLibName, const std::string &mtlName);
};

} }
