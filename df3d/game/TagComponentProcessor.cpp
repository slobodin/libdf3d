#include "TagComponentProcessor.h"

#include <df3d/lib/Utils.h>

namespace df3d {

const std::unordered_set<Entity>& TagComponentProcessor::getEntities(int tag)
{
    return m_entities[tag];
}

const std::unordered_set<int>* TagComponentProcessor::getTags(Entity e)
{
    auto found = m_tagLookup.find(e);
    if (found != m_tagLookup.end())
        return &found->second;
    return nullptr;
}

Entity TagComponentProcessor::getFirst(int tag)
{
    const auto &wTag = getEntities(tag);
    if (wTag.empty())
        return{};
    return *wTag.begin();
}

bool TagComponentProcessor::hasTag(Entity e, int tag) const
{
    auto found = m_tagLookup.find(e);
    return found != m_tagLookup.end() && utils::contains_key(found->second, tag);
}

void TagComponentProcessor::add(Entity e, int tag)
{
    m_tagLookup[e].insert(tag);
    m_entities[tag].insert(e);
}

void TagComponentProcessor::remove(Entity e)
{
    auto tags = m_tagLookup.find(e);
    if (tags != m_tagLookup.end())
    {
        for (auto tag : tags->second)
        {
            auto arrFound = m_entities.find(tag);
            DF3D_ASSERT(arrFound != m_entities.end());

            arrFound->second.erase(e);
        }

        DF3D_VERIFY(m_tagLookup.erase(e) == 1);
    }
}

bool TagComponentProcessor::has(Entity e)
{
    return utils::contains_key(m_tagLookup, e);
}

}
