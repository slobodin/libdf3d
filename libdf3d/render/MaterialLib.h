#pragma once

#include "resources/Resource.h"

namespace df3d { namespace render {

class Material;

class DF3D_DLL MaterialLib : public resources::Resource
{
    std::unordered_map<std::string, shared_ptr<Material>> m_materials;
public:
    MaterialLib();
    ~MaterialLib();

    bool init();

    shared_ptr<Material> getMaterial(const char *name);
    void appendMaterial(shared_ptr<Material> material);

    bool isMaterialExists(const char *name);
    size_t materialCount() const;

    // FIXME:
    static shared_ptr<Material> getMaterial(const char *mtlLibName, const char *mtlName);
};

} }