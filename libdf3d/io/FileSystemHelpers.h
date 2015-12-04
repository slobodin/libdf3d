#pragma once

namespace df3d {

class FileDataSource;

struct FileSystemHelpers
{
    static bool isPathAbsolute(const std::string &path);
    static bool pathExists(const std::string &path);
    static shared_ptr<FileDataSource> openFile(const std::string &path);
};

}
