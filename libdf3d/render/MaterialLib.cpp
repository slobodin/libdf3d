#include "df3d_pch.h"
#include "MaterialLib.h"

#include "Material.h"
#include <base/Controller.h>
#include <resources/ResourceManager.h>

namespace df3d { namespace render {

std::vector<std::string> MaterialLib::Defines;

MaterialLib::MaterialLib()
{

}

MaterialLib::~MaterialLib()
{

}

bool MaterialLib::init()
{
    return m_materials.size() != 0;
}

shared_ptr<Material> MaterialLib::getMaterial(const char *name)
{
    auto found = m_materials.find(name);
    if (found == m_materials.end())
    {
        base::glog << "Material with name" << name << "wasn't found in material library" << m_guid << base::logwarn;
        return nullptr;
    }

    return found->second;
}

void MaterialLib::appendMaterial(shared_ptr<Material> material)
{
    auto found = m_materials.find(material->getName());
    if (found != m_materials.end())
    {
        base::glog << "Trying to add duplicate material" << found->first << "to material library" << m_guid << base::logwarn;
        return;
    }

    m_materials[material->getName()] = material;

    if (material->getTechniquesCount() == 0)
    {
        base::glog << "Material" << material->getName() << "without techniques has been added to library" << m_guid << base::logwarn;
    }
}

bool MaterialLib::isMaterialExists(const char *name)
{
    return m_materials.find(name) != m_materials.end();
}

size_t MaterialLib::materialCount() const
{
    return m_materials.size();
}

shared_ptr<Material> MaterialLib::getMaterial(const char *mtlLibName, const char *mtlName)
{
    auto mtlLib = g_resourceManager->getResource<MaterialLib>(mtlLibName);
    if (!mtlLib)
        return nullptr;
    return mtlLib->getMaterial(mtlName);
}

} }