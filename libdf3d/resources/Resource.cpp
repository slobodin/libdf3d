#include "Resource.h"

#include <base/EngineController.h>
#include <resources/FileSystem.h>

namespace df3d {

Resource::Resource()
    : m_initialized(false),
    m_resident(false)
{
    static unsigned long resourcesCount = 0;

    std::ostringstream ss;
    ss << "unnamed_resource_" << resourcesCount++;
    m_guid = ss.str();
}

void Resource::setGUID(const ResourceGUID &guid)
{ 
    if (!IsGUIDValid(guid))
    {
        glog << "Trying to set invalid guid" << guid << logwarn;
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
