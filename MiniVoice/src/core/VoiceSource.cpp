#include "core/VoiceSource.hpp"
#include "core/VoicePlayer.hpp"
#include "utils/Helper.hpp"

namespace core
{
    VoiceSource::VoiceSource(float volume, std::shared_ptr<VoicePlayer> voicePlayer)
    {
        this->volume = volume;
        this->voicePlayer = voicePlayer;

        samplesList = std::make_shared<std::queue<std::shared_ptr<float[]>>>();
    }

    float VoiceSource::getVolume() const
    {
        return volume;
    }

    void VoiceSource::setVolume(float volume)
    {
        this->volume = volume;
    }

    void VoiceSource::enqueueSamples(std::shared_ptr<float[]> samples)
    {
        ma_apply_volume_factor_pcm_frames_f32(samples.get(), utils::getTotalBytes(voicePlayer->getSampleRate(), voicePlayer->getFrameSizeMS(), voicePlayer->getChannels(), voicePlayer->getBytesPerSample()), voicePlayer->getChannels(), volume);

        samplesList->emplace(samples);
    }

    std::optional<std::shared_ptr<float[]>> VoiceSource::dequeueSamples()
    {
        if (samplesList->empty())
        {
            return std::nullopt;
        }

        std::shared_ptr<float[]> samples = samplesList->front();

        samplesList->pop();

        return samples;
    }
}