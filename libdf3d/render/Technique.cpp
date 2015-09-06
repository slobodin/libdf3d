#include "df3d_pch.h"
#include "Technique.h"

#include "RenderPass.h"

namespace df3d { namespace render {

RenderPass* Technique::findPass(const std::string &name)
{
    auto findFn = [&](const RenderPass &pass)
    {
        return pass.getName() == name;
    };

    auto found = std::find_if(m_passes.begin(), m_passes.end(), findFn);
    if (found == m_passes.end())
        return nullptr;

    return &(*found);
}

Technique::Technique(const std::string &name)
    : m_name(name)
{
}

Technique::~Technique()
{
}

void Technique::appendPass(const RenderPass &pass)
{
    m_passes.push_back(pass);
}

RenderPass* Technique::getPass(int idx)
{
    try
    {
        return &m_passes.at(idx);
    }
    catch (std::out_of_range &) { }

    return nullptr;
}

RenderPass* Technique::getPass(const std::string &name)
{
    return findPass(name);
}

size_t Technique::getPassCount() const
{
    return m_passes.size();
}

const std::string &Technique::getName() const
{
    return m_name;
}

} }
