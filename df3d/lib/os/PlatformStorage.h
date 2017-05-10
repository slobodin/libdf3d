#pragma once

namespace df3d {

class PlatformStorage
{
public:
    static bool saveData(const char *id, const PodArray<uint8_t> &data);
    static void getData(const char *id, PodArray<uint8_t> &data);
};

}
