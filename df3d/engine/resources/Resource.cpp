#include "Resource.h"

#include <df3d/engine/io/FileSystemHelpers.h>

namespace df3d {

Resource::Resource()
    : m_initialized(false),
    m_resident(false)
{
    static int32_t resourcesCount = 0;

    std::ostringstream ss;
    ss << "unnamed_resource_" << resourcesCount++;
    m_guid = ss.str();
}

void Resource::setGUID(const ResourceGUID &guid)
{ 
    m_guid = guid; 
}

const ResourceGUID &Resource::getGUID() const
{ 
    return m_guid; 
}

const std::string &Resource::getFilePath() const
{
    return m_guid;
}

ResourceGUID CreateGUIDFromPath(const std::string &path)
{
    auto res = path;
    FileSystemHelpers::convertSeparators(res);
    return res;
}

}
