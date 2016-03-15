#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>
#include <vector>

#include <libdf3d/df3d.h>
#include <libdf3d/resources/ResourceContainer.h>

static_assert(sizeof(typename std::string::value_type) == 1, "Invalid string size");

template<typename T>
void Serialize(const T &data, std::ofstream &fs)
{
    fs.write(reinterpret_cast<const char *>(&data), sizeof(data));

    if (!fs)
        throw std::runtime_error("failed to write to an output");
}

void Serialize(const void *data, size_t size, std::ofstream &fs)
{
    if (size == 0)
        return;
    fs.write(reinterpret_cast<const char *>(data), size);

    if (!fs)
        throw std::runtime_error("failed to write to an output");
}

size_t GetFileLen(std::istream &is)
{
    is.seekg(0, is.end);
    auto length = is.tellg();
    is.seekg(0, is.beg);

    return length;
}

int main(int argc, const char **argv) try
{
    std::cout << "data_archiver starting" << std::endl;

    df3d::FileSystem df3dFs;

    if (argc != 3)
        throw std::runtime_error("invalid input. Usage: data_archiver.exe file_with_list_of_files_to_archive output_filename");

    std::ofstream output(argv[2], std::ios::out | std::ios::binary);
    if (!output)
        throw std::runtime_error("failed to open output file.");

    std::ifstream inputFile(argv[1]);
    if (!inputFile)
        throw std::runtime_error("failed to open input file.");

    output.seekp(sizeof(df3d::ResourceContainer::Header));

    long offsetCounter = output.tellp();
    std::vector<df3d::ResourceContainer::Entry> entries;
    std::string currentFileName;
    while (std::getline(inputFile, currentFileName))
    {
        currentFileName = df3dFs.fullPath(currentFileName);
        if (currentFileName.empty())
            throw std::runtime_error("invalid input filename");

        if (currentFileName.size() >= df3d::ResourceContainer::MAX_FILENAME_LEN)
            throw std::runtime_error("input filename is too big");

        std::ifstream currentFile(currentFileName, std::ios::in | std::ios::binary);
        if (!currentFile)
            throw std::runtime_error("failed to open file being archived");

        std::cout << "writing " << currentFileName << std::endl;

        df3d::ResourceContainer::Entry entry;
        entry.length = GetFileLen(currentFile);
        entry.offset = offsetCounter;
        memcpy(entry.fileName, currentFileName.c_str(), currentFileName.size());

        entries.push_back(entry);

        offsetCounter += entry.length;

        // Write the data.
        char *buffer = new char[entry.length];
        currentFile.read(buffer, entry.length);

        if (!currentFile)
            throw std::runtime_error("failed to read input file data");

        Serialize(buffer, entry.length, output);

        delete[] buffer;
    }

    // Write entries.
    for (const auto &entry : entries)
        Serialize(entry, output);

    output.seekp(0);

    df3d::ResourceContainer::Header header;
    memset(&header, 0, sizeof(header));

    header.magic = *((uint32_t*)df3d::ResourceContainer::MAGIC);
    header.version = 1;
    header.entriesCount = entries.size();
    header.entriesOffset = offsetCounter;

    // Write header.
    Serialize(header, output);

    std::cout << "Done!" << std::endl;

    return 0;
}
catch (std::exception &e)
{
    std::cerr << "An error occurred:\n" << e.what() << "\n";

    // TODO: remove result file if any.

    return 1;
}
