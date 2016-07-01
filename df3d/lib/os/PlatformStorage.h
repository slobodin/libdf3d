#pragma once

namespace df3d {

class DF3D_DLL PlatformStorage
{
public:
    static bool saveData(const char *id, const std::vector<uint8_t> &data);
    static void getData(const char *id, std::vector<uint8_t> &data);
};

}
