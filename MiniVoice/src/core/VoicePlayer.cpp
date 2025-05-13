#include "core/VoicePlayer.hpp"
#include "utils/Helper.hpp"
#include <cstring>
#include <ranges>

#include "core/VoiceSource.hpp"

namespace core
{
    VoicePlayer::VoicePlayer(float volume, int sampleRate, int channels, int frameSizeMS) : VoiceBase(volume, sampleRate, channels, frameSizeMS)
    {
        voiceSources = std::make_shared<std::map<int, std::shared_ptr<VoiceSource>>>();

        mixedSamples = std::make_shared<float[]>(utils::getTotalBytes(sampleRate, frameSizeMS, channels, bytesPerSample) / sizeof(float));

        init(sampleRate, channels, frameSizeMS, std::nullopt);
    }

    void staticWriteSamples(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount)
    {
        VoicePlayer* currentVoicePlayer = static_cast<VoicePlayer*>(pDevice->pUserData);

        int totalSamplesBytes = utils::getTotalBytes(currentVoicePlayer->sampleRate , currentVoicePlayer->frameSizeMS, currentVoicePlayer->channels, currentVoicePlayer->bytesPerSample);

        for (const std::pair<int, std::shared_ptr<VoiceSource>> voiceSource : *currentVoicePlayer->voiceSources)
        {
            std::optional<std::shared_ptr<float[]>> voiceSourceSamples = voiceSource.second->dequeueSamples();

            if (!voiceSourceSamples.has_value())
            {
                continue;
            }

            ma_mix_pcm_frames_f32(currentVoicePlayer->mixedSamples.get(), voiceSourceSamples.value().get(), totalSamplesBytes / sizeof(float) / currentVoicePlayer->channels, currentVoicePlayer->channels, currentVoicePlayer->volume);
        }

        memcpy(pOutput, currentVoicePlayer->mixedSamples.get(), totalSamplesBytes);

        memset(currentVoicePlayer->mixedSamples.get(), 0, totalSamplesBytes);
    }

    void VoicePlayer::init(int sampleRate, int channels, int frameSizeMS, const std::optional<std::string>& playbackDevice)
    {
        if (alreadyInitialized)
        {
            ma_context_uninit(context.get());
            ma_device_stop(device.get());
            ma_device_uninit(device.get());
        }

        context = std::make_shared<ma_context>();

        ma_result contextInitResult = ma_context_init(nullptr, 0, nullptr, context.get());

        if (contextInitResult != MA_SUCCESS)
        {
            throw std::runtime_error("Failed to initialize context");
        }

        ma_device_config deviceConfig = ma_device_config_init(ma_device_type_playback);

        if (playbackDevice.has_value())
        {
            refreshAudioDeviceMapping();

            if (audioDevicesMapping == nullptr)
            {
                throw std::runtime_error("This shouldn't happened audioDevicesMapping should have been initialized");
            }

            if (!audioDevicesMapping->contains(playbackDevice.value()))
            {
                throw std::runtime_error("There is no recording device with the name " + playbackDevice.value() + "\n");
            }

            deviceConfig.playback.pDeviceID = &audioDevicesMapping->at(playbackDevice.value());
        }

        deviceConfig.playback.format = ma_format_f32;
        deviceConfig.playback.channels = channels;
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
            throw std::runtime_error("Failed to initialize playback device");
        }

        if (alreadyInitialized && isPlaying)
        {
            ma_result deviceStartResult = ma_device_start(device.get());

            if (deviceStartResult != MA_SUCCESS)
            {
                throw std::runtime_error("Failed to start device");
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
            throw std::runtime_error("Failed to call ma_context_get_devices()");
        }

        for (int i = 0; i < playbackCount; i++)
        {
            audioDevicesMapping->insert({ playbackInfos[i].name, playbackInfos[i].id });
        }
    }

    void VoicePlayer::addVoiceSource(int id)
    {
        voiceSources->emplace(id, std::make_shared<VoiceSource>(1, std::shared_ptr<VoicePlayer>(this)));
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
            throw std::runtime_error("Cannot get device name");
        }

        ma_result deviceGetNameResultSecond = ma_device_get_name(device.get(), ma_device_type_playback, deviceName.get(), nameLength + 1, nullptr);

        if (deviceGetNameResultSecond != MA_SUCCESS)
        {
            throw std::runtime_error("Cannot get device name");
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
            throw std::runtime_error("Failed to start device");
        }
    }

    void VoicePlayer::stopPlaying()
    {
        isPlaying = false;

        ma_device_stop(device.get());
    }
}
