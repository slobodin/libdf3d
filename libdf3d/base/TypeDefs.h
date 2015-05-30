#pragma once

#include <string>

namespace df3d
{

namespace scene { class Node; }

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
using SceneNode = shared_ptr<df3d::scene::Node>;
using WeakSceneNode = weak_ptr<df3d::scene::Node>;

}
