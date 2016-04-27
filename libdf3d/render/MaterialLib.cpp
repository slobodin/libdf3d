#include "MaterialLib.h"

#include "Material.h"
#include <libdf3d/base/EngineController.h>

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
        glog << "Material with name" << name << "wasn't found in material library" << getGUID() << logwarn;
        return nullptr;
    }

    return found->second;
}

void MaterialLib::appendMaterial(shared_ptr<Material> material)
{
    auto found = m_materials.find(material->getName());
    if (found != m_materials.end())
    {
        glog << "Trying to add duplicate material" << found->first << "to material library" << getGUID() << logwarn;
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
        glog << "Material without techniques" << material->getName() << "has been added to library" << getGUID() << logwarn;
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
