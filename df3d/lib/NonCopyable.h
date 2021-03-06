#pragma once

namespace df3d {

class NonCopyable
{
protected:
    NonCopyable() = default;
    ~NonCopyable() = default;

    NonCopyable(const NonCopyable &other) = delete;
    NonCopyable& operator= (const NonCopyable &other) = delete;
};

}
