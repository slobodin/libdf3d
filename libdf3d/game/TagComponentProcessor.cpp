#include "TagComponentProcessor.h"

#include <utils/Utils.h>

namespace df3d {    

void TagComponentProcessor::update()
{

}

void TagComponentProcessor::cleanStep(const std::list<Entity> &deleted)
{
    for (auto &e : deleted)
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

bool TagComponentProcessor::hasTag(Entity e, int tag) const
{
    auto found = m_tagLookup.find(e);
    return found != m_tagLookup.end() && found->second == tag;
}

void TagComponentProcessor::add(Entity e, int tag)
{
    if (utils::contains_key(m_tagLookup, e))
    {
        glog << "An entity already has a tag component" << logwarn;
        return;
    }

    m_tagLookup.insert(std::make_pair(e, tag));
    m_entities[tag].insert(e);
}

void TagComponentProcessor::remove(Entity e)
{
    auto tag = m_tagLookup.find(e);
    if (tag != m_tagLookup.end())
    {
        auto arrFound = m_entities.find(tag->second);
        assert(arrFound != m_entities.end());

        arrFound->second.erase(e);
        m_tagLookup.erase(tag);
    }
}

}