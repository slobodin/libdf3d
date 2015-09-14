#include "df3d_pch.h"
#include "Technique.h"

#include "RenderPass.h"

namespace df3d { namespace render {

shared_ptr<RenderPass> Technique::findPass(const std::string &name)
{
    auto findFn = [&](const shared_ptr<RenderPass> &pass)
    {
        return pass->getName() == name;
    };

    auto found = std::find_if(m_passes.begin(), m_passes.end(), findFn);
    if (found == m_passes.end())
        return nullptr;

    return *found;
}

Technique::Technique(const std::string &name)
    : m_name(name)
{
}

Technique::Technique(const Technique &other)
    : m_name(other.m_name)
{
    for (const auto &pass : other.m_passes)
        m_passes.push_back(make_shared<RenderPass>(*pass));
}

Technique& Technique::operator= (Technique other)
{
    std::swap(m_name, other.m_name);
    std::swap(m_passes, other.m_passes);

    return *this;
}

Technique::~Technique()
{
}

void Technique::appendPass(const RenderPass &pass)
{
    m_passes.push_back(make_shared<RenderPass>(pass));
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

const std::string &Technique::getName() const
{
    return m_name;
}

} }
