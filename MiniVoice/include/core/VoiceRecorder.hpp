#pragma once

#include "../MiniVoiceExport.hpp"
#include "../externals/miniaudio.h"
#include <map>
#include <iostream>
#include <memory>
#include <optional>
#include <queue>
#include <vector>
#include "core/VoiceBase.hpp"

namespace core
{
	class MINIVOICE_API VoiceRecorder : public VoiceBase
	{
	public:
		VoiceRecorder(float volume, int sampleRate, int channels, int frameSizeMS);

		std::shared_ptr<std::vector<std::string>> getRecordingDeviceNames();
		void setCurrentRecordingDevice(const std::optional<std::string>& name);

		[[nodiscard]] std::string getCurrentRecordingDeviceName() const;

		void setVolume(float volume) const;
		void startRecording();
		void stopRecording();

		std::optional<std::shared_ptr<float[]>> dequeueSamples() const;

	private:
		bool alreadyInitialized = false;
		bool isRecording = false;

		std::shared_ptr<std::queue<std::shared_ptr<float[]>>> samplesList = nullptr;
		std::shared_ptr<std::map<std::string, ma_device_id>> audioDevicesMapping = nullptr;

		std::shared_ptr<ma_device> device = nullptr;
		std::shared_ptr<ma_context> context = nullptr;

		void init(int sampleRate, int channels, int frameSizeMS, const std::optional<std::string>& recordingDevice);
		void refreshAudioDeviceMapping();

		friend void staticReadSamples(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount);
	};
}