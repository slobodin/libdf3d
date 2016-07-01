#include "MaterialLib.h"

#include "Material.h"
#include <df3d/engine/EngineController.h>

namespace df3d {

std::string MaterialLib::PREFERRED_TECHNIQUE;
std::set<std::string> MaterialLib::SHADER_DEFINES;

MaterialLib::MaterialLib()
{

}

MaterialLib::~MaterialLib()
{

}

shared_ptr<Material> MaterialLib::getMaterial(const std::string &name) const
{
    auto found = m_materials.find(name);
    if (found == m_materials.end())
    {
        DFLOG_WARN("Material with name %s wasn't found in material library %s", name.c_str(), getGUID().c_str());
        return nullptr;
    }

    return found->second;
}

void MaterialLib::appendMaterial(shared_ptr<Material> material)
{
    auto found = m_materials.find(material->getName());
    if (found != m_materials.end())
    {
        DFLOG_WARN("Trying to add duplicate material %s to material library %s", found->first.c_str(), getGUID().c_str());
        return;
    }

    if (!PREFERRED_TECHNIQUE.empty())
    {
        auto tech = material->getTechnique(PREFERRED_TECHNIQUE);
        if (tech)
            material->setCurrentTechnique(PREFERRED_TECHNIQUE);
    }

    m_materials[material->getName()] = material;

    if (material->getTechniquesCount() == 0)
        DFLOG_WARN("Material %s without techniques has been added to library %s", material->getName().c_str(), getGUID().c_str());
}

bool MaterialLib::isMaterialExists(const std::string &name)
{
    return m_materials.find(name) != m_materials.end();
}

size_t MaterialLib::materialsCount() const
{
    return m_materials.size();
}

}
