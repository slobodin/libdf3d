#include "Utils.h"

#include <zlib.h>

namespace df3d { namespace utils {

bool inflateUncompress(uint8_t *dest, size_t destLen, const uint8_t *source, size_t sourceLen)
{
    uLongf tmp = destLen;

    if (uncompress(dest, &tmp, source, sourceLen) != Z_OK)
        return false;

    return tmp == destLen;
}

} }
