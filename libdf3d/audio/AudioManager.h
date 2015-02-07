#pragma once

FWD_MODULE_CLASS(base, EngineController)

namespace df3d { namespace audio {

class AudioNode;

class DF3D_DLL AudioManager
{
    friend class base::EngineController;
    friend class AudioNode;

    struct Impl;
    unique_ptr<Impl> m_pimpl;

    AudioManager();
    ~AudioManager();

    // Only for controller.
    void update(float dt);

public:

};

} }