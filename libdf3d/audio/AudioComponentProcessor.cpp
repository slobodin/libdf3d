#include "df3d_pch.h"
#include "AudioComponentProcessor.h"

namespace df3d {

AudioComponentProcessor::AudioComponentProcessor()
{

}

AudioComponentProcessor::~AudioComponentProcessor()
{

}

void AudioComponentProcessor::play(ComponentInstance e)
{

}

void AudioComponentProcessor::stop(ComponentInstance e)
{

}

void AudioComponentProcessor::pause(ComponentInstance e)
{

}

void AudioComponentProcessor::setPitch(ComponentInstance e, float pitch)
{

}

void AudioComponentProcessor::setGain(ComponentInstance e, float gain)
{

}

void AudioComponentProcessor::setLooped(ComponentInstance e, bool looped)
{

}

float AudioComponentProcessor::getPitch(ComponentInstance e) const
{
    return 0.0f;
}

float AudioComponentProcessor::getGain(ComponentInstance e) const
{
    return 0.0f;
}

bool AudioComponentProcessor::isLooped(ComponentInstance e) const
{
    return false;
}

ComponentInstance AudioComponentProcessor::get(Entity e)
{
    return ComponentInstance();
}

ComponentInstance AudioComponentProcessor::add(Entity e)
{
    return ComponentInstance();
}

void AudioComponentProcessor::remove(Entity e)
{

}

}

