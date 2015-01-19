#pragma once

namespace df3d { namespace platform {

struct FileSystemHelpers
{
    static bool isPathAbsolute(const std::string &path);
    static bool pathExists(const std::string &path);
};

} }