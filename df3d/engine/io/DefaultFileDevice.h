#pragma once

#include "FileSystem.h"

namespace df3d {

class DF3D_DLL DefaultFileDevice : public IFileDevice
{
public:
    DefaultFileDevice();
    ~DefaultFileDevice();

    shared_ptr<DataSource> openDataSource(const char *path) override;
};

}
