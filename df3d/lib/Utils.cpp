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
        if (m_a >= m_b)
            return m_a;
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
        if (m_a >= m_b)
            return m_a;
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
    if (a == b)
        return a;
    UniformRealDistribution dis(a, b);
    return dis(gen);
}

float RandomUtils::randRange(const glm::vec2 &a)
{
    return randRange(a.x, a.y);
}

int RandomUtils::randRange(int a, int b)
{
    if (a == b)
        return a;
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

TimeUtils::TimePoint TimeUtils::now()
{
    return std::chrono::system_clock::now();
}

namespace utils {

bool inflateUncompress(uint8_t *dest, size_t destLen, const uint8_t *source, size_t sourceLen)
{
    uLongf tmp = destLen;

    if (uncompress(dest, &tmp, source, sourceLen) != Z_OK)
        return false;

    return tmp == destLen;
}

std::vector<uint8_t> zlibCompress(const std::vector<uint8_t> &input)
{
    std::vector<uint8_t> result;

    z_stream zs;
    memset(&zs, 0, sizeof(zs));

    if (deflateInit(&zs, Z_BEST_COMPRESSION) != Z_OK)
    {
        DFLOG_WARN("Failed to zlibCompress");
        return result;
    }

    zs.next_in = (Bytef*)input.data();
    zs.avail_in = input.size();

    int ret;
    char outbuffer[32768];

    // retrieve the compressed bytes blockwise
    do {
        zs.next_out = reinterpret_cast<Bytef*>(outbuffer);
        zs.avail_out = sizeof(outbuffer);

        ret = deflate(&zs, Z_FINISH);

        if (result.size() < zs.total_out)
        {
            // append the block to the output string
            result.insert(result.end(), outbuffer, outbuffer + (zs.total_out - result.size()));
        }
    } while (ret == Z_OK);

    deflateEnd(&zs);

    if (ret != Z_STREAM_END) 
    {
        DFLOG_WARN("zlibCompress error: %s", zs.msg);
        result.clear();
        return result;
    }

    return result;
}

std::vector<uint8_t> zlibDecompress(const std::vector<uint8_t> &input)
{
    std::vector<uint8_t> result;

    z_stream zs;
    memset(&zs, 0, sizeof(zs));

    if (inflateInit(&zs) != Z_OK)
    {
        DFLOG_WARN("Failed to init z_stream");
        return result;
    }

    zs.next_in = (Bytef*)input.data();
    zs.avail_in = input.size();

    int ret;
    char outbuffer[32768];

    // get the decompressed bytes blockwise using repeated calls to inflate
    do
    {
        zs.next_out = reinterpret_cast<Bytef*>(outbuffer);
        zs.avail_out = sizeof(outbuffer);

        ret = inflate(&zs, 0);

        if (result.size() < zs.total_out)
        {
            result.insert(result.end(), outbuffer, outbuffer + (zs.total_out - result.size()));
        }

    } while (ret == Z_OK);

    inflateEnd(&zs);

    if (ret != Z_STREAM_END)
    {
        DFLOG_WARN("zlibDecompress error: %s", zs.msg);
        result.clear();
        return result;
    }

    return result;
}

} }
