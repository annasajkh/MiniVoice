#include "core/VoicePlayer.hpp"
#include "utils/Helper.hpp"
#include <cstring>
#include <ranges>
#include <thread>

#include "core/VoiceSource.hpp"

namespace core
{
    VoicePlayer::VoicePlayer(float volume, int sampleRate, int channels, int frameSizeMS) : VoiceBase(volume, sampleRate, channels, frameSizeMS)
    {
        voiceSources = std::make_shared<std::map<int, std::shared_ptr<VoiceSource>>>();
        mixedSamples = std::make_shared<float[]>(4096 * channels);

        context = std::make_shared<ma_context>();

        ma_result contextResult = ma_context_init(nullptr, 0, nullptr, context.get());
        if (contextResult != MA_SUCCESS) {
            throw std::runtime_error("Failed to initialize context. Error: " + utils::maResultToString(contextResult));
        }

        init(sampleRate, channels, frameSizeMS, std::nullopt);
    }

    void staticWriteSamples(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount)
    {
        VoicePlayer* currentVoicePlayer = static_cast<VoicePlayer*>(pDevice->pUserData);

        const size_t frameBytes = frameCount * currentVoicePlayer->channels * sizeof(float);

        for (const std::pair<int, std::shared_ptr<VoiceSource>> voiceSource : *currentVoicePlayer->voiceSources)
        {
            std::optional<std::shared_ptr<float[]>> voiceSourceSamples = voiceSource.second->dequeueSamples();

            if (!voiceSourceSamples.has_value())
            {
                continue;
            }

            ma_mix_pcm_frames_f32(currentVoicePlayer->mixedSamples.get(), voiceSourceSamples.value().get(), frameBytes / sizeof(float) / currentVoicePlayer->channels, currentVoicePlayer->channels, currentVoicePlayer->volume);
        }

        memcpy(pOutput, currentVoicePlayer->mixedSamples.get(), frameBytes);

        memset(currentVoicePlayer->mixedSamples.get(), 0, frameBytes);
    }

    void VoicePlayer::init(int sampleRate, int channels, int frameSizeMS, const std::optional<std::string>& playbackDevice)
    {
        if (alreadyInitialized)
        {
            ma_device_stop(device.get());
            ma_device_uninit(device.get());
        }

        ma_device_config deviceConfig = ma_device_config_init(ma_device_type_playback);

        if (playbackDevice.has_value())
        {
            refreshAudioDeviceMapping();

            if (audioDevicesMapping == nullptr)
            {
                throw std::runtime_error("This shouldn't happened audioDevicesMapping should already have been initialized");
            }

            if (!audioDevicesMapping->contains(playbackDevice.value()))
            {
                throw std::runtime_error("There is no playback device with the name " + playbackDevice.value() + "\n");
            }

            deviceConfig.playback.pDeviceID = &audioDevicesMapping->at(playbackDevice.value());
        }

        deviceConfig.playback.format = ma_format_f32;
        deviceConfig.playback.channels = channels;
        deviceConfig.playback.shareMode = ma_share_mode_shared;
        deviceConfig.sampleRate = sampleRate;
        deviceConfig.periodSizeInMilliseconds = frameSizeMS;
        deviceConfig.dataCallback = &staticWriteSamples;
        deviceConfig.pUserData = this;

        device = std::make_shared<ma_device>();

        if (context == nullptr || device == nullptr)
        {
            throw std::runtime_error("This shouldn't happened, context and device should have value!");
        }

        ma_result deviceInitResult = ma_device_init(context.get(), &deviceConfig, device.get());

        if (deviceInitResult != MA_SUCCESS)
        {
            throw std::runtime_error("Failed to initialize playback device. Error: " + utils::maResultToString(deviceInitResult));
        }

        if (alreadyInitialized && isPlaying)
        {
            ma_result deviceStartResult = ma_device_start(device.get());

            if (deviceStartResult != MA_SUCCESS)
            {
                throw std::runtime_error("Failed to start device. Error: " + utils::maResultToString(deviceStartResult));
            }
        }

        alreadyInitialized = true;
    }

    void VoicePlayer::refreshAudioDeviceMapping()
    {
        audioDevicesMapping = std::make_shared<std::map<std::string, ma_device_id>>();

        ma_device_info* playbackInfos;
        ma_uint32 playbackCount;

        if (context == nullptr)
        {
            throw std::runtime_error("This shouldn't happened, context should have value!");
        }

        ma_result contextGetDevicesResult = ma_context_get_devices(context.get(), &playbackInfos, &playbackCount, nullptr, nullptr);

        if (contextGetDevicesResult != MA_SUCCESS)
        {
            throw std::runtime_error("Failed to call ma_context_get_devices(). Error: " + utils::maResultToString(contextGetDevicesResult));
        }

        for (int i = 0; i < playbackCount; i++)
        {
            audioDevicesMapping->insert({ playbackInfos[i].name, playbackInfos[i].id });
        }
    }

    void VoicePlayer::addVoiceSource(int id)
    {
        voiceSources->emplace(id, std::make_shared<VoiceSource>(1, this));
    }

    void VoicePlayer::removeVoiceSource(int id) const
    {
        voiceSources->erase(id);
    }

    void VoicePlayer::enqueueSample(int id, std::shared_ptr<float[]> samples) const
    {
        voiceSources->at(id)->enqueueSamples(samples);
    }

    std::shared_ptr<std::vector<std::string>> VoicePlayer::getPlaybackDeviceNames()
    {
        std::shared_ptr deviceNames = std::make_shared<std::vector<std::string>>();

        refreshAudioDeviceMapping();

        for (const std::string& key : *audioDevicesMapping | std::views::keys)
        {
            deviceNames->emplace_back(key);
        }

        return deviceNames;
    }

    void VoicePlayer::setCurrentPlaybackDevice(const std::optional<std::string>& name)
    {
        init(sampleRate, channels, frameSizeMS, name);
    }

    std::string VoicePlayer::getCurrentPlaybackDeviceName() const
    {
        size_t nameLength;

        ma_result deviceGetNameResultFirst = ma_device_get_name(device.get(), ma_device_type_playback, nullptr, 0, &nameLength);

        std::unique_ptr<char[]> deviceName = std::make_unique<char[]>(nameLength + 1);

        if (deviceGetNameResultFirst != MA_SUCCESS)
        {
            throw std::runtime_error("Cannot get device name. Error: " + utils::maResultToString(deviceGetNameResultFirst));
        }

        ma_result deviceGetNameResultSecond = ma_device_get_name(device.get(), ma_device_type_playback, deviceName.get(), nameLength + 1, nullptr);

        if (deviceGetNameResultSecond != MA_SUCCESS)
        {
            throw std::runtime_error("Cannot get device name. Error: " + utils::maResultToString(deviceGetNameResultSecond));
        }

        return { deviceName.get() };
    }

    void VoicePlayer::setVolume(float volume)
    {
        this->volume = volume;
    }

    void VoicePlayer::startPlaying()
    {
        isPlaying = true;

        ma_result deviceStartResult = ma_device_start(device.get());

        if (deviceStartResult != MA_SUCCESS)
        {
            throw std::runtime_error("Failed to start device. Error: " + utils::maResultToString(deviceStartResult));
        }
    }

    void VoicePlayer::stopPlaying()
    {
        isPlaying = false;

        ma_device_stop(device.get());
    }

    VoicePlayer::~VoicePlayer()
    {
        if (device) {
            ma_device_stop(device.get());
            ma_device_uninit(device.get());
        }

        if (context) {
            ma_context_uninit(context.get());
        }
    }
}
