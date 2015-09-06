#include "df3d_pch.h"
#include "Material.h"

#include "Technique.h"

namespace df3d { namespace render {

Technique* Material::findTechnique(const std::string &name)
{
    auto findFn = [&](const Technique &tech)
    {
        return tech.getName() == name;
    };

    auto found = std::find_if(m_techniques.begin(), m_techniques.end(), findFn);
    if (found == m_techniques.end())
        return nullptr;
    
    return &(*found);
}

Material::Material(const std::string &name)
    : m_name(name)
{
    if (name.empty())
        base::glog << "Creating material with empty name" << base::logwarn;
}

Material::~Material()
{

}

const std::string &Material::getName() const
{
    return m_name;
}

void Material::appendTechnique(const Technique &technique)
{
    if (findTechnique(technique.getName()))
    {
        base::glog << "Trying to add duplicate technique" << technique.getName() << "to material" << m_name << base::logwarn;
        return;
    }

    m_techniques.push_back(technique);
}

void Material::setCurrentTechnique(const std::string &name)
{
    if (m_currentTechnique && m_currentTechnique->getName() == name)
        return;

    auto found = findTechnique(name);
    if (found)
        m_currentTechnique = found;
    else
        base::glog << "Trying to set empty technique" << name << "to material" << m_name << base::logwarn;
}

Technique* Material::getCurrentTechnique()
{
    return m_currentTechnique;
}

Technique* Material::getTechnique(const std::string &name)
{
    return findTechnique(name);
}

size_t Material::getTechniquesCount() const
{
    return m_techniques.size();
}

} }
