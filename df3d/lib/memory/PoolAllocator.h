#pragma once

#include "Allocator.h"

namespace df3d {

class PoolAllocator : public Allocator
{
public:
    PoolAllocator();
    ~PoolAllocator();
};

}
