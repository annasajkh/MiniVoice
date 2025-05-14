#pragma once
#include "../MiniVoiceExport.hpp"

#include "core/VoiceBase.hpp"
#include <memory>
#include <vector>
#include <map>
#include <iostream>
#include <optional>
#include "../externals/miniaudio.h"

namespace core
{
    class VoiceSource;

    void staticWriteSamples(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount);

    class MINIVOICE_API VoicePlayer : public VoiceBase
    {
    public:
        VoicePlayer(float volume, int sampleRate, int channels, int frameSizeMS);

        std::shared_ptr<std::map<int, std::shared_ptr<VoiceSource>>> voiceSources;

        void addVoiceSource(int id);
        void removeVoiceSource(int id) const;
        void enqueueSample(int id, std::shared_ptr<float[]> samples) const;

        std::shared_ptr<std::vector<std::string>> getPlaybackDeviceNames();
        void setCurrentPlaybackDevice(const std::optional<std::string>& name);

        [[nodiscard]] std::string getCurrentPlaybackDeviceName() const;

        void setVolume(float volume);
        void startPlaying();
        void stopPlaying();

        ~VoicePlayer();

    private:
        bool alreadyInitialized = false;
        bool isPlaying = false;

        std::shared_ptr<float[]> mixedSamples;
        std::shared_ptr<std::map<std::string, ma_device_id>> audioDevicesMapping = nullptr;

        std::shared_ptr<ma_device> device = nullptr;
        std::shared_ptr<ma_context> context = nullptr;

        void init(int sampleRate, int channels, int frameSizeMS, const std::optional<std::string>& playbackDevice);
        void refreshAudioDeviceMapping();

        friend void staticWriteSamples(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount);
    };
}