#pragma once

namespace df3d {

class GPUMemStats
{
    int64_t m_memUsed = 0;

public:
    GPUMemStats() = default;
    ~GPUMemStats() = default;

    void traceAlloc(const char *tag, uint32_t size);
    void traceFree(const char *tag, uint32_t size);
    uint32_t getMemTotal() const { return (uint32_t)m_memUsed; }
};

}
