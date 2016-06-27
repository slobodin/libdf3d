#include "Utils.h"

#include <zlib.h>

namespace df3d { namespace utils {

std::mt19937_64& RNG()
{
    static std::mt19937_64 gen(std::chrono::high_resolution_clock::now().time_since_epoch().count());
    return gen;
}

bool inflateUncompress(uint8_t *dest, size_t destLen, const uint8_t *source, size_t sourceLen)
{
    uLongf tmp = destLen;

    if (uncompress(dest, &tmp, source, sourceLen) != Z_OK)
        return false;

    return tmp == destLen;
}

} }
