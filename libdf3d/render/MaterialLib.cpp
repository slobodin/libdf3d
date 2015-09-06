#include "df3d_pch.h"
#include "MaterialLib.h"

#include "Material.h"
#include <base/SystemsMacro.h>
#include <resources/ResourceFactory.h>

namespace df3d { namespace render {

std::vector<std::string> MaterialLib::Defines;

void MaterialLib::onDecoded(bool decodeResult)
{
    // TODO_REFACTO

    assert(false);
}

MaterialLib::MaterialLib()
{

}

MaterialLib::~MaterialLib()
{

}

const Material* MaterialLib::getMaterial(const std::string &name) const
{
    auto found = m_materials.find(name);
    if (found == m_materials.end())
    {
        base::glog << "Material with name" << name << "wasn't found in material library" << getGUID() << base::logwarn;
        return nullptr;
    }

    return &found->second;
}

Material* MaterialLib::getMaterial(const std::string &name)
{
    // TODO_REFACTO this is fucking disgusting.
    return (Material*)const_cast<const MaterialLib*>(this)->getMaterial(name);
}

void MaterialLib::appendMaterial(const Material &material)
{
    auto found = m_materials.find(material.getName());
    if (found != m_materials.end())
    {
        base::glog << "Trying to add duplicate material" << found->first << "to material library" << getGUID() << base::logwarn;
        return;
    }

    m_materials[material.getName()] = material;

    if (material.getTechniquesCount() == 0)
        base::glog << "Material without techniques" << material.getName() << "has been added to library" << getGUID() << base::logwarn;
}

bool MaterialLib::isMaterialExists(const std::string &name)
{
    return m_materials.find(name) != m_materials.end();
}

size_t MaterialLib::materialsCount() const
{
    return m_materials.size();
}

Material* MaterialLib::getMaterial(const std::string &mtlLibName, const std::string &mtlName)
{
    // TODO_REFACTO remove async materiallib loading.

    auto mtlLib = g_resourceManager->getFactory().createMaterialLib(mtlLibName);
    if (!mtlLib)
        return nullptr;
    return mtlLib->getMaterial(mtlName);
}

} }
