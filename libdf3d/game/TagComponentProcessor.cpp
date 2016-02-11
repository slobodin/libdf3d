#include "TagComponentProcessor.h"

#include <libdf3d/utils/Utils.h>

namespace df3d {

void TagComponentProcessor::update()
{

}

void TagComponentProcessor::cleanStep(const std::list<Entity> &deleted)
{
    for (auto e : deleted)
        remove(e);
}

TagComponentProcessor::TagComponentProcessor()
{

}

TagComponentProcessor::~TagComponentProcessor()
{

}

const std::unordered_set<Entity>& TagComponentProcessor::getEntities(int tag)
{
    return m_entities[tag];
}

Entity TagComponentProcessor::getFirst(int tag)
{
    const auto &wTag = getEntities(tag);
    if (wTag.empty())
        return {};
    return *wTag.begin();
}

bool TagComponentProcessor::hasTag(Entity e, int tag) const
{
    auto found = m_tagLookup.find(e);
    return found != m_tagLookup.end() && df3d::utils::contains_key(found->second, tag);
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
            assert(arrFound != m_entities.end());

            arrFound->second.erase(e);
        }

        assert(m_tagLookup.erase(e) == 1);
    }
}

}
