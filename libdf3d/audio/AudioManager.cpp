#include "df3d_pch.h"
#include "AudioManager.h"

#include "OpenALCommon.h"
#include <base/SystemsMacro.h>
#include <components/TransformComponent.h>
#include <scene/SceneManager.h>
#include <scene/Camera.h>
#include <utils/Utils.h>

namespace df3d { namespace audio {

struct AudioManager::Impl
{
    ALCdevice *m_device = nullptr;
    ALCcontext *m_context = nullptr;
};

AudioManager::AudioManager()
    : m_pimpl(new AudioManager::Impl())
{
    base::glog << "Initializing OpenAL" << base::logmess;

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

    base::glog << "Available playback devices:" << devices << base::logmess;
    base::glog << "Default playback device:" << defaultDevice << base::logmess;

    m_pimpl->m_device = alcOpenDevice(nullptr);
    if (!m_pimpl->m_device)
        throw std::runtime_error("Can not open audio device");

    m_pimpl->m_context = alcCreateContext(m_pimpl->m_device, nullptr);
    if (!m_pimpl->m_context)
        throw std::runtime_error("Can not create OpenAL context");

    alcMakeContextCurrent(m_pimpl->m_context);

    printOpenALError();
}

AudioManager::~AudioManager()
{
    alcMakeContextCurrent(nullptr);
    alcDestroyContext(m_pimpl->m_context);
    alcCloseDevice(m_pimpl->m_device);
}

void AudioManager::update(float dt)
{
    auto cam = g_sceneManager->getCamera();
    if (cam)
    {
        alListenerfv(AL_POSITION, glm::value_ptr(cam->transform()->getPosition()));

        const auto &dir = cam->getDir();
        const auto &up = cam->getUp();

        ALfloat listenerOrientation[] = { dir.x, dir.y, dir.z, up.x, up.y, up.z };
        alListenerfv(AL_ORIENTATION, listenerOrientation);
    }
}

} }
