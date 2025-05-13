#pragma once

#include "../MiniVoiceExport.hpp"

namespace core
{
    class MINIVOICE_API VoiceBase
    {
    public:
        VoiceBase(float volume, int sampleRate, int channels, int frameSizeMS);

        [[nodiscard]] float getVolume() const;
        [[nodiscard]] int getSampleRate() const;
        [[nodiscard]] int getChannels() const;
        [[nodiscard]] int getFrameSizeMS() const;
        [[nodiscard]] int getBytesPerSample() const;

    protected:
        float volume;
        int sampleRate;
        int channels;
        int frameSizeMS;
        int bytesPerSample;
    };
}