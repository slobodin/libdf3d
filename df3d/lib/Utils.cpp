#include "Utils.h"

#include <zlib.h>

namespace df3d {

class UniformRealDistribution
{
    float m_a;
    float m_b;

public:
    UniformRealDistribution(float a, float b)
        :m_a(a),
        m_b(b)
    { }

    template <class Generator>
    float operator()(Generator &g)
    {
        float dScale = (m_b - m_a) / ((float)(g.max() - g.min()) + 1.0f);
        return (g() - g.min()) * dScale + m_a;
    }
};

class UniformIntDistribution
{
    int m_a;
    int m_b;

public:
    UniformIntDistribution(int a, int b)
        :m_a(a),
        m_b(b)
    { }

    template <class Generator>
    int operator()(Generator &g)
    {
        return m_a + g() / (g.max() / (m_b - m_a + 1) + 1);
    }
};

std::mt19937 RandomUtils::gen;

void RandomUtils::srand()
{
    gen = std::mt19937(std::chrono::high_resolution_clock::now().time_since_epoch().count());
}

void RandomUtils::srand(std::mt19937::result_type seed)
{
    gen = std::mt19937(seed);
}

int RandomUtils::rand()
{
    return randRange(0, 32767);
}

float RandomUtils::randRange(float a, float b)
{
    UniformRealDistribution dis(a, b);
    return dis(gen);
}

int RandomUtils::randRange(int a, int b)
{
    UniformIntDistribution dis(a, b);
    return dis(gen);
}

float TimeUtils::IntervalBetween(const TimePoint &t1, const TimePoint &t2)
{
    using namespace std::chrono;
    return duration_cast<milliseconds>(t1 - t2).count() / 1000.f;
}

float TimeUtils::IntervalBetweenNowAnd(const TimePoint &timepoint)
{
    using namespace std::chrono;
    return duration_cast<milliseconds>(system_clock::now() - timepoint).count() / 1000.0f;
}

namespace utils {

bool inflateUncompress(uint8_t *dest, size_t destLen, const uint8_t *source, size_t sourceLen)
{
    uLongf tmp = destLen;

    if (uncompress(dest, &tmp, source, sourceLen) != Z_OK)
        return false;

    return tmp == destLen;
}

} }
