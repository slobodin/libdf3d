#pragma once

#include <base/EngineModule.h>

namespace df3d {

class DF3D_DLL AudioManager : public EngineModule
{
    struct Impl;
    unique_ptr<Impl> m_pimpl;

    AudioManager();
    ~AudioManager();

    void update(float systemDelta, float gameDelta) override;

public:

};

}
