#pragma once

#include <df3d/engine/io/FileDataSource.h>

namespace df3d {

// TODO: encrypt entries table.

class DF3D_DLL ResourceContainer : NonCopyable
{
public:
    static const int MAX_FILENAME_LEN = 128;

#pragma pack(push, 1)
    struct Entry
    {
        // 4 Gb max.
        int32_t offset;
        int32_t length;
        char fileName[MAX_FILENAME_LEN] = { 0 };
    };

    struct Header
    {
        uint32_t magic;
        uint16_t version;

        uint32_t entriesOffset;
        uint32_t entriesCount;
    };
#pragma pack(pop)

    static const char MAGIC[4];

private:
    shared_ptr<FileDataSource> m_archiveFile;
    std::vector<Entry> m_entries;

public:
    ResourceContainer(shared_ptr<FileDataSource> archiveFile);
    ~ResourceContainer();

    shared_ptr<FileDataSource> getFile() { return m_archiveFile; }
    const std::vector<Entry>& getEntries() const { return m_entries; }
};

//class ResourceContainerDataSource : public FileDataSource
//{
//    weak_ptr<FileDataSource> m_archiveFile;
//    ResourceContainer::Entry m_entry;
//    size_t m_internalPos = 0;
//
//public:
//    ResourceContainerDataSource(weak_ptr<FileDataSource> archiveFile, const ResourceContainer::Entry &entry);
//    ~ResourceContainerDataSource();
//
//    bool valid() const override;
//
//    size_t getRaw(void *buffer, size_t sizeInBytes) override;
//    size_t getSizeInBytes() override;
//
//    size_t tell() override;
//    bool seek(size_t offset, std::ios_base::seekdir origin) override;
//};

}
