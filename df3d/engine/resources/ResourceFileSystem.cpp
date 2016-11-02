#include "ResourceFileSystem.h"

#include "ResourceDataSource.h"

namespace df3d {

class DefaultFileSystem : public ResourceFileSystem
{
    std::recursive_mutex m_lock;
    std::list<unique_ptr<ResourceDataSource>> m_sources;

public:
    DefaultFileSystem() = default;
    ~DefaultFileSystem()
    {
        DF3D_ASSERT(m_sources.empty());
    }

    ResourceDataSource* open(const char *path)
    {
        std::lock_guard<std::recursive_mutex> lock(m_lock);

        auto dataSource = CreateFileDataSource(path);
        if (!dataSource)
            return nullptr;

        m_sources.push_back(std::move(dataSource));

        return m_sources.back().get();
    }

    void close(ResourceDataSource *dataSource)
    {
        std::lock_guard<std::recursive_mutex> lock(m_lock);

        auto found = std::find_if(m_sources.begin(), m_sources.end(), [dataSource](const unique_ptr<ResourceDataSource> &other) {
            return dataSource == other.get();
        });

        if (found != m_sources.end())
            m_sources.erase(found);
        else
            DFLOG_WARN("Failed to close resource data source");
    }
};

unique_ptr<ResourceFileSystem> CreateDefaultResourceFileSystem()
{
    return make_unique<DefaultFileSystem>();
}

}
