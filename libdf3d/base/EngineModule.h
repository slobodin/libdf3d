#pragma once

#include <base/EngineInitParams.h>

namespace df3d {

class DF3D_DLL EngineModule : private utils::NonCopyable
{
public:
    virtual ~EngineModule() = default;

    virtual void init(const EngineInitParams &params) = 0;
    virtual void shutdown() = 0;

    virtual void update(float systemDelta, float gameDelta) = 0;
    virtual void render() = 0;
};

}
