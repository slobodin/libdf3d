#include "df3d_pch.h"
#include "Technique.h"

#include "RenderPass.h"

namespace df3d { namespace render {

shared_ptr<RenderPass> Technique::findPass(const std::string &name) const
{
    auto findFn = [&](const shared_ptr<RenderPass> pass)
    {
        return pass->getName() == name;
    };

    auto found = std::find_if(m_passes.cbegin(), m_passes.cend(), findFn);
    if (found == m_passes.cend())
        return nullptr;

    return *found;
}

void Technique::appendPass(shared_ptr<RenderPass> pass)
{
    if (!pass)
    {
        base::glog << "Trying to add empty pass to technique" << m_name << base::logwarn;
        return;
    }

    m_passes.push_back(pass);
}

Technique::Technique(const std::string &name)
    : m_name(name)
{
}

Technique::~Technique()
{
}

shared_ptr<RenderPass> Technique::getPass(int idx)
{
    try
    {
        return m_passes.at(idx);
    }
    catch (std::out_of_range &) { }

    return nullptr;
}

shared_ptr<RenderPass> Technique::getPass(const std::string &name)
{
    return findPass(name);
}

size_t Technique::getPassCount() const
{
    return m_passes.size();
}

const std::string &Technique::getName()
{
    return m_name;
}

} }
