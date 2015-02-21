#pragma once

#include <string>

namespace df3d
{

enum class ResourceLoadingMode
{
    IMMEDIATE,
    ASYNC
};

enum class PixelFormat
{
    INVALID,
    RGB,
    BGR,
    RGBA,
    GRAYSCALE
};

using ResourceGUID = std::string;

}
