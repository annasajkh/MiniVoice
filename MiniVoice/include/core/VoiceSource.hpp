#pragma once

#include "../MiniVoiceExport.hpp"
#include <memory>
#include <queue>
#include "VoicePlayer.hpp"
#include <optional>

namespace core
{
    class VoicePlayer;

    class MINIVOICE_API VoiceSource
    {
    public:
        VoiceSource(float volume, std::shared_ptr<VoicePlayer> voicePlayer);

        void enqueueSamples(std::shared_ptr<float[]> samples);
        std::optional<std::shared_ptr<float[]>> dequeueSamples();

        [[nodiscard]] float getVolume() const;
        
        void setVolume(float volume);

    private:
        float volume;

        std::shared_ptr<std::queue<std::shared_ptr<float[]>>> samplesList = nullptr;
        std::shared_ptr<VoicePlayer> voicePlayer;
    };

}