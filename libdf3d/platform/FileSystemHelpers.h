#pragma once

FWD_MODULE_CLASS(resources, FileDataSource);

namespace df3d { namespace platform {

struct FileSystemHelpers
{
    static bool isPathAbsolute(const std::string &path);
    static bool pathExists(const std::string &path);
    static shared_ptr<resources::FileDataSource> openFile(const std::string &path);
};

} }
