#include "AudioManager.h"

#include "OpenALCommon.h"

namespace df3d {

struct AudioManager::Impl
{
    ALCdevice *m_device = nullptr;
    ALCcontext *m_context = nullptr;
};

AudioManager::AudioManager()
{

}

AudioManager::~AudioManager()
{

}

void AudioManager::initialize()
{
    DFLOG_MESS("Initializing OpenAL");

    m_pimpl = make_unique<Impl>();

#ifdef _DEBUG
#if defined(DF3D_WINDOWS)
    //_putenv_s("ALSOFT_LOGLEVEL", "3");
#endif
#endif

    std::string devices;
    if (alcIsExtensionPresent(NULL, "ALC_ENUMERATE_ALL_EXT") != AL_FALSE)
        devices = alcGetString(NULL, ALC_ALL_DEVICES_SPECIFIER);
    else
        devices = alcGetString(NULL, ALC_DEVICE_SPECIFIER);

    std::string defaultDevice;
    if (alcIsExtensionPresent(NULL, "ALC_ENUMERATE_ALL_EXT") != AL_FALSE)
        defaultDevice = alcGetString(NULL, ALC_DEFAULT_ALL_DEVICES_SPECIFIER);
    else
        defaultDevice = alcGetString(NULL, ALC_DEFAULT_DEVICE_SPECIFIER);

    DFLOG_MESS("Available playback devices: %s", devices.c_str());
    DFLOG_MESS("Default playback device: %s", defaultDevice.c_str());

    m_pimpl->m_device = alcOpenDevice(nullptr);
    if (!m_pimpl->m_device)
        throw std::runtime_error("Can not open audio device");

    m_pimpl->m_context = alcCreateContext(m_pimpl->m_device, nullptr);
    if (!m_pimpl->m_context)
        throw std::runtime_error("Can not create OpenAL context");

    alcMakeContextCurrent(m_pimpl->m_context);

    printOpenALError();
}

void AudioManager::shutdown()
{
    alcMakeContextCurrent(nullptr);
    alcDestroyContext(m_pimpl->m_context);
    alcCloseDevice(m_pimpl->m_device);
    m_pimpl.reset();
}

void AudioManager::suspend()
{
#ifdef ALC_SOFT_pause_device
    DFLOG_DEBUG("Doing alcDevicePauseSOFT");
    alcDevicePauseSOFT(m_pimpl->m_device);
#endif
}

void AudioManager::resume()
{
    alcMakeContextCurrent(m_pimpl->m_context);
#ifdef ALC_SOFT_pause_device
    DFLOG_DEBUG("Doing alcDeviceResumeSOFT");
    alcDeviceResumeSOFT(m_pimpl->m_device);
#endif
}

}
