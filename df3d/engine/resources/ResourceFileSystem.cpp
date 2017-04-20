#include "ResourceFileSystem.h"

#include "ResourceDataSource.h"
#include <df3d/engine/EngineController.h>
#include <df3d/lib/Utils.h>
#include <mutex>
#include <unordered_set>
#include <df3d/lib/assert/Assert.h>
#include <df3d/lib/memory/Allocator.h>
#include <df3d/lib/Log.h>

namespace df3d {

class DefaultFileSystem : public ResourceFileSystem
{
    std::recursive_mutex m_lock;
    std::unordered_set<ResourceDataSource*> m_sources;

public:
    DefaultFileSystem() = default;
    ~DefaultFileSystem()
    {
        DF3D_ASSERT(m_sources.empty());
    }

    ResourceDataSource* open(const char *path)
    {
        std::lock_guard<std::recursive_mutex> lock(m_lock);

        auto dataSource = CreateFileDataSource(path, MemoryManager::allocDefault());
        if (!dataSource)
            return nullptr;

        DF3D_ASSERT(!utils::contains(m_sources, dataSource));
        m_sources.insert(dataSource);

        return dataSource;
    }

    void close(ResourceDataSource *dataSource)
    {
        std::lock_guard<std::recursive_mutex> lock(m_lock);

        auto found = m_sources.find(dataSource);

        if (found != m_sources.end())
        {
            MAKE_DELETE(MemoryManager::allocDefault(), *found);
            m_sources.erase(found);
        }
        else
            DFLOG_WARN("Failed to close resource data source");
    }
};

unique_ptr<ResourceFileSystem> CreateDefaultResourceFileSystem()
{
    return make_unique<DefaultFileSystem>();
}

}
