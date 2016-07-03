#pragma once

#include "Allocator.h"

namespace df3d {

class DF3D_DLL PoolAllocator : public Allocator
{
public:
    PoolAllocator();
    ~PoolAllocator();
};

}
