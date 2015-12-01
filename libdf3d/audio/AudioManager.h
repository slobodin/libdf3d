#pragma once

namespace df3d {

class EngineController;

class DF3D_DLL AudioManager : utils::NonCopyable
{
    friend class EngineController;

    struct Impl;
    unique_ptr<Impl> m_pimpl;

    AudioManager();
    ~AudioManager();

    void update(float systemDelta, float gameDelta);

public:

};

}
