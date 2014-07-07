#pragma once

FWD_MODULE_CLASS(base, Controller)

namespace df3d { namespace audio {

class AudioNode;

class DF3D_DLL AudioManager
{
    friend class base::Controller;
    friend class AudioNode;

    struct Impl;
    scoped_ptr<Impl> m_pimpl;

    AudioManager();
    ~AudioManager();

    // These are only for controller.
    bool init();
    void shutdown();

    void update(float dt);
public:

};

} }