#include "df3d_pch.h"
#include "Vertex.h"

#include <boost/algorithm/string.hpp>

#include "OpenGLCommon.h"
#include <utils/Utils.h>

namespace df3d { namespace render {

void VertexFormat::addComponent(const VertexComponent &component)
{
    m_components.push_back(component);
    m_vertexSize += component.getCount() * sizeof(float);
}

void VertexFormat::enableGLAttributes() const
{
    size_t offset = 0;
    for (const auto &c : m_components)
    {
        glEnableVertexAttribArray(c.getType());

        glVertexAttribPointer(c.getType(), c.getCount(), GL_FLOAT, GL_FALSE, m_vertexSize, (void *)offset);

        offset += c.getCount() * sizeof(float);
    }
}

void VertexFormat::disableGLAttributes() const
{
    for (const auto &c : m_components)
        glDisableVertexAttribArray(c.getType());
}

size_t VertexFormat::getOffsetTo(VertexComponent::Type component) const
{
    size_t offset = 0;
    for (const auto &c : m_components)
    {
        if (c.getType() == component)
            return offset;

        offset += c.getCount() * sizeof(float);
    }

    // No such component.
    base::glog << "Invalid component type passed to VertexFormat::getOffsetTo" << base::logwarn;
    return 0;
}

bool VertexFormat::hasComponent(VertexComponent::Type component) const
{
    return getComponent(component) != nullptr;
}

const VertexComponent *VertexFormat::getComponent(VertexComponent::Type component) const
{
    auto found = std::find_if(m_components.cbegin(), m_components.cend(), [&](const VertexComponent &c) { return c.getType() == component; });
    if (found == m_components.end())
        return nullptr;

    return &(*found);
}

VertexFormat VertexFormat::create(const std::string &definition)
{
    VertexFormat retRes;

    std::vector<std::string> components;
    boost::split(components, definition, boost::is_any_of(", "));

    for (const auto &c : components)
    {
        if (c.empty())
            continue;

        auto colon = c.find(':');
        auto name = c.substr(0, colon);
        auto count = c.substr(colon + 1);

        VertexComponent::Type t;
        if (name == "p")
            t = VertexComponent::POSITION;
        else if (name == "n")
            t = VertexComponent::NORMAL;
        else if (name == "tx")
            t = VertexComponent::TEXTURE_COORDS;
        else if (name == "c")
            t = VertexComponent::COLOR;
        else if (name == "tan")
            t = VertexComponent::TANGENT;
        else if (name == "bitan")
            t = VertexComponent::BITANGENT;
        else
        {
            base::glog << "Invalid vertex format" << definition << base::logwarn;
            return retRes;
        }

        VertexComponent component(t, utils::from_string<size_t>(count));
        
        retRes.addComponent(component);
    }

    return retRes;
}

} }
