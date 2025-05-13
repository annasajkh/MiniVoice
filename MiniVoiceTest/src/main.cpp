#include "core/VoiceRecorder.hpp"
#include "core/VoiceSource.hpp"
#include "core/VoicePlayer.hpp"

using namespace core;

int main()
{
    std::shared_ptr<VoiceRecorder> recorder = std::make_shared<VoiceRecorder>(1, 48000, 2, 20);
    std::shared_ptr<VoicePlayer> player = std::make_shared<VoicePlayer>(1, 48000, 2, 20);

    std::shared_ptr<std::vector<std::string>> recordingDeviceNames = recorder->getRecordingDeviceNames();
    std::shared_ptr<std::vector<std::string>> playbackDeviceNames = player->getPlaybackDeviceNames();

    player->setVolume(2);
    player->addVoiceSource(0);

    std::cout << "------------------------------------------------------------------------------------------------------\n";

    for (std::string& deviceName: *recordingDeviceNames)
    {
        std::cout << "Available recording device: " << deviceName << "\n";
    }

    std::cout << "------------------------------------------------------------------------------------------------------\n";

    for (std::string& deviceName : *playbackDeviceNames)
    {
        std::cout << "Available playback device: " << deviceName << "\n";
    }

    std::cout << "------------------------------------------------------------------------------------------------------\n";


    std::cout << "Starting mic loopback you can try to talk now... \n";

    recorder->startRecording();
    player->startPlaying();

    while (true)
    {
        std::optional<std::shared_ptr<float[]>> samples = recorder->dequeueSamples();

        if (!samples.has_value())
        {
            continue;
        }

        player->enqueueSample(0, samples.value());
    }
}