#pragma once

namespace df3d {

class DF3D_DLL AudioManager : utils::NonCopyable
{
    struct Impl;
    unique_ptr<Impl> m_pimpl;

public:
    AudioManager();
    ~AudioManager();

    void initialize();
    void shutdown();
};

}
