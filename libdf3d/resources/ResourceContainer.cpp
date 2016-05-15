#include "ResourceContainer.h"

namespace df3d {

const char ResourceContainer::MAGIC[4] = { 'D', 'F', 'R', 'E' };

ResourceContainer::ResourceContainer(shared_ptr<FileDataSource> archiveFile)
    : m_archiveFile(archiveFile)
{
    DF3D_ASSERT(m_archiveFile, "sanity check");

    Header header;
    m_archiveFile->getAsObjects(&header, 1);

    if (strncmp((const char *)&header.magic, "DFRE", 4) != 0)
    {
        DFLOG_WARN("Invalid df3d resource container magic");
        return;
    }

    m_archiveFile->seek(header.entriesOffset, std::ios_base::beg);
    m_entries.resize(header.entriesCount);
    m_archiveFile->getAsObjects(&m_entries[0], header.entriesCount);
}

ResourceContainer::~ResourceContainer()
{

}

//ResourceContainerDataSource::ResourceContainerDataSource(weak_ptr<FileDataSource> archiveFile, const ResourceContainer::Entry &entry)
//    : FileDataSource(entry.fileName),
//    m_archiveFile(archiveFile),
//    m_entry(entry)
//{
//
//}

//ResourceContainerDataSource::~ResourceContainerDataSource()
//{
//
//}
//
//bool ResourceContainerDataSource::valid() const
//{
//    return m_archiveFile.lock() != nullptr;
//}
//
//size_t ResourceContainerDataSource::getRaw(void *buffer, size_t sizeInBytes)
//{
//    if (m_internalPos + sizeInBytes > m_entry.length)
//        sizeInBytes = m_entry.length - m_internalPos;
//
//    m_archiveFile.lock()->seek(m_entry.offset + m_internalPos, std::ios_base::beg);
//
//    auto read = m_archiveFile.lock()->getRaw(buffer, sizeInBytes);
//
//    m_internalPos += read;
//
//    return read;
//}
//
//size_t ResourceContainerDataSource::getSizeInBytes()
//{
//    return m_entry.length;
//}
//
//size_t ResourceContainerDataSource::tell()
//{
//    return m_internalPos;
//}
//
//bool ResourceContainerDataSource::seek(size_t offset, std::ios_base::seekdir origin)
//{
//    if (origin == std::ios_base::cur)
//        m_internalPos += offset;
//    else if (origin == std::ios_base::beg)
//        m_internalPos = offset;
//    else if (origin == std::ios_base::end)
//        m_internalPos = m_entry.length - offset - 1;
//    else
//        return false;
//
//    return m_internalPos >= 0 && m_internalPos < m_entry.length;
//}

}
