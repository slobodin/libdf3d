#include "Resource.h"

#include <df3d/engine/EngineController.h>
#include <df3d/engine/io/FileSystem.h>

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
    if (!IsGUIDValid(guid))
    {
        DFLOG_WARN("Trying to set invalid guid %s", guid.c_str());
        return;
    }

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

bool IsGUIDValid(const ResourceGUID &guid)
{
    return !guid.empty();
}

ResourceGUID CreateGUIDFromPath(const std::string &path)
{
    return svc().fileSystem().fullPath(path);
}

}
