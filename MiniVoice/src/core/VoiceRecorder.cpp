
#include "core/VoiceRecorder.hpp"
#include <optional>
#include <ranges>
#include <utility>
#include <cstring>
#include <thread>

#include "utils/Helper.hpp"

namespace core
{
    VoiceRecorder::VoiceRecorder(float volume, int sampleRate, int channels, int frameSizeMS) : VoiceBase(volume, sampleRate, channels, frameSizeMS)
    {
        audioDevicesMapping = std::make_shared<std::map<std::string, ma_device_id>>();
        samplesList = std::make_shared<std::queue<std::shared_ptr<float[]>>>();

        context = std::make_shared<ma_context>();

        ma_result contextResult = ma_context_init(nullptr, 0, nullptr, context.get());
        if (contextResult != MA_SUCCESS) {
            throw std::runtime_error("Failed to initialize context. Error: " + utils::maResultToString(contextResult));
        }

        init(sampleRate, channels, frameSizeMS, std::nullopt);
    }

    void staticReadSamples(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount)
    {
        VoiceRecorder* currentVoiceRecorder = static_cast<VoiceRecorder*>(pDevice->pUserData);

        if (currentVoiceRecorder != nullptr && currentVoiceRecorder->samplesList != nullptr && pInput != nullptr)
        {
            size_t dataSize = frameCount * currentVoiceRecorder->getChannels() * sizeof(float);

            std::shared_ptr<float[]> buffer = std::make_shared<float[]>(frameCount * currentVoiceRecorder->getChannels());

            memcpy(buffer.get(), pInput, dataSize);

            currentVoiceRecorder->samplesList->emplace(buffer);
        }
    }

    void VoiceRecorder::init(int sampleRate, int channels, int frameSizeMS, const std::optional<std::string>& recordingDevice)
    {
        if (alreadyInitialized)
        {
            ma_device_stop(device.get());
            ma_device_uninit(device.get());
        }

        ma_device_config deviceConfig = ma_device_config_init(ma_device_type_capture);

        if (recordingDevice.has_value())
        {
            refreshAudioDeviceMapping();

            if (audioDevicesMapping == nullptr)
            {
                throw std::runtime_error("This shouldn't happened audioDevicesMapping should have been initialized");
            }

            if (!audioDevicesMapping->contains(recordingDevice.value()))
            {
                throw std::runtime_error("There is no recording device with the name " + recordingDevice.value() + "\n");
            }

            deviceConfig.capture.pDeviceID = &audioDevicesMapping->at(recordingDevice.value());
        }

        deviceConfig.capture.format = ma_format_f32;
        deviceConfig.capture.channels = channels;
        deviceConfig.capture.shareMode = ma_share_mode_shared;
        deviceConfig.sampleRate = sampleRate;
        deviceConfig.periodSizeInMilliseconds = frameSizeMS;
        deviceConfig.dataCallback = &staticReadSamples;
        deviceConfig.pUserData = this;

        device = std::make_shared<ma_device>();

        if (context == nullptr || device == nullptr)
        {
            throw std::runtime_error("This shouldn't happened, context and device should have value!");
        }

        ma_result deviceInitResult = ma_device_init(context.get(), &deviceConfig, device.get());

        if (deviceInitResult != MA_SUCCESS)
        {
            throw std::runtime_error("Failed to initialize capture device. Error: " + utils::maResultToString(deviceInitResult));
        }

        if (alreadyInitialized && isRecording)
        {
            ma_result deviceStartResult = ma_device_start(device.get());

            if (deviceStartResult != MA_SUCCESS)
            {
                throw std::runtime_error("Failed to start device. Error: " + utils::maResultToString(deviceStartResult));
            }
        }

        alreadyInitialized = true;
    }

    std::shared_ptr<std::vector<std::string>> VoiceRecorder::getRecordingDeviceNames()
    {
        std::shared_ptr deviceNames = std::make_shared<std::vector<std::string>>();

        refreshAudioDeviceMapping();

        for(const std::string& key: *audioDevicesMapping | std::views::keys)
        {
            deviceNames->emplace_back(key);
        }

        return deviceNames;
    }

    void VoiceRecorder::setCurrentRecordingDevice(const std::optional<std::string>& name)
    {
        init(sampleRate, channels, frameSizeMS, name);
    }

    std::string VoiceRecorder::getCurrentRecordingDeviceName() const
    {
        size_t nameLength;

        ma_result deviceGetNameResultFirst = ma_device_get_name(device.get(), ma_device_type_capture, nullptr, 0, &nameLength);

        std::unique_ptr<char[]> deviceName = std::make_unique<char[]>(nameLength + 1);

        if (deviceGetNameResultFirst != MA_SUCCESS)
        {
            throw std::runtime_error("Cannot get device name. Error: " + utils::maResultToString(deviceGetNameResultFirst));
        }

        ma_result deviceGetNameResultSecond = ma_device_get_name(device.get(), ma_device_type_capture, deviceName.get(), nameLength + 1, nullptr);

        if (deviceGetNameResultSecond != MA_SUCCESS)
        {
            throw std::runtime_error("Cannot get device name. Error: " + utils::maResultToString(deviceGetNameResultSecond));
        }

        return { deviceName.get() };
    }

    void VoiceRecorder::setVolume(float volume) const
    {
        ma_device_set_master_volume(device.get(), volume);
    }

    void VoiceRecorder::startRecording()
    {
        isRecording = true;

        ma_result deviceStartResult = ma_device_start(device.get());

        if (deviceStartResult != MA_SUCCESS)
        {
            throw std::runtime_error("Failed to start device. Error: " + utils::maResultToString(deviceStartResult));
        }
    }

    void VoiceRecorder::stopRecording()
    {
        isRecording = false;

        ma_device_stop(device.get());
    }

    std::optional<std::shared_ptr<float[]>> VoiceRecorder::dequeueSamples() const
    {
        if (samplesList->empty())
        {
            return std::nullopt;
        }

        std::shared_ptr<float[]> samples = samplesList->front();

        samplesList->pop();

        return samples;
    }

    void VoiceRecorder::refreshAudioDeviceMapping()
    {
        audioDevicesMapping = std::make_shared<std::map<std::string, ma_device_id>>();

        ma_device_info* captureInfos;
        ma_uint32 captureCount;

        if (context == nullptr)
        {
            throw std::runtime_error("This shouldn't happened, context should have value!");
        }

        ma_result contextGetDevicesResult = ma_context_get_devices(context.get(), nullptr, nullptr, &captureInfos, &captureCount);

        if (contextGetDevicesResult != MA_SUCCESS)
        {
            throw std::runtime_error("Failed to call ma_context_get_devices(). Error: " + utils::maResultToString(contextGetDevicesResult));
        }

        for (int i = 0; i < captureCount; i++)
        {
            audioDevicesMapping->insert({ captureInfos[i].name, captureInfos[i].id });
        }
    }

    VoiceRecorder::~VoiceRecorder()
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
