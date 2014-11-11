#include "df3d_pch.h"
#include "Material.h"

#include "Technique.h"

namespace df3d { namespace render {

shared_ptr<Technique> Material::findTechnique(const std::string &name) const
{
    auto findFn = [&](const shared_ptr<Technique> tech)
    {
        return tech->getName() == name;
    };

    auto found = std::find_if(m_techniques.cbegin(), m_techniques.cend(), findFn);
    if (found == m_techniques.cend())
        return nullptr;
    
    return *found;
}

void Material::appendTechnique(shared_ptr<Technique> technique)
{
    if (!technique)
    {
        base::glog << "Trying to add empty technique to material" << m_name << base::logwarn;
        return;
    }

    if (findTechnique(technique->getName()))
    {
        base::glog << "Trying to add duplicate technique" << technique->getName() << "to material" << m_name << base::logwarn;
        return;
    }

    m_techniques.push_back(technique);
}

Material::Material(const std::string &name)
    : m_name(name)
{

}

Material::~Material()
{

}

const std::string &Material::getName() const
{
    return m_name;
}

void Material::setCurrentTechnique(const char *name)
{
    if (m_currentTechnique && m_currentTechnique->getName() == name)
        return;

    auto found = findTechnique(name);
    if (found)
        m_currentTechnique = found;
    else
        base::glog << "Trying to set empty technique" << name << "to material" << m_name << base::logwarn;
}

shared_ptr<Technique> Material::getCurrentTechnique()
{
    return m_currentTechnique;
}

shared_ptr<Technique> Material::getTechnique(const char *name)
{
    // TODO:
    // Maybe create technique if not found?
    return findTechnique(name);
}

size_t Material::getTechniquesCount() const
{
    return m_techniques.size();
}

} }