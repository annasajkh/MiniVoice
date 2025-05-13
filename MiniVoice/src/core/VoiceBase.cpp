#include "core/VoiceBase.hpp"

#define MINIAUDIO_IMPLEMENTATION
#include <miniaudio.h>

namespace core
{
    VoiceBase::VoiceBase(float volume, int sampleRate, int channels, int frameSizeMS)
    {
        this->volume = volume;
        this->sampleRate = sampleRate;
        this->channels = channels;
        this->frameSizeMS = frameSizeMS;
        this->bytesPerSample = ma_get_bytes_per_sample(ma_format_f32);
    }

    float VoiceBase::getVolume() const
    {
        return volume;
    }

    int VoiceBase::getSampleRate() const
    {
        return sampleRate;
    }

    int VoiceBase::getChannels() const
    {
        return channels;
    }

    int VoiceBase::getFrameSizeMS() const
    {
        return frameSizeMS;
    }

    int VoiceBase::getBytesPerSample() const
    {
        return bytesPerSample;
    }
}