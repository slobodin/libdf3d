#include "GPUMemStats.h"

namespace df3d {

void GPUMemStats::traceAlloc(const char *tag, uint32_t size)
{
    m_memUsed += size;
}

void GPUMemStats::traceFree(const char *tag, uint32_t size)
{
    m_memUsed -= size;
    DF3D_ASSERT(m_memUsed >= 0);
}

}
