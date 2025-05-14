#include <thread>
#include <iostream>
#include <atomic>
#include <chrono>

#include "core/VoiceRecorder.hpp"
#include "core/VoiceSource.hpp"
#include "core/VoicePlayer.hpp"

using namespace core;

std::atomic<bool> running(true);

void readSampleFunction(std::shared_ptr<VoiceRecorder> recorder, std::shared_ptr<VoicePlayer> player)
{
    while (running)
    {
        std::optional<std::shared_ptr<float[]>> samples = recorder->dequeueSamples();

        if (!samples.has_value())
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            continue;
        }

        player->enqueueSample(0, samples.value());
    }
}

int main()
{
    std::shared_ptr<VoiceRecorder> recorder = std::make_shared<VoiceRecorder>(1, 48000, 2, 20);
    std::shared_ptr<VoicePlayer> player = std::make_shared<VoicePlayer>(1, 48000, 2, 20);
    std::shared_ptr<std::vector<std::string>> recordingDeviceNames = recorder->getRecordingDeviceNames();
    std::shared_ptr<std::vector<std::string>> playbackDeviceNames = player->getPlaybackDeviceNames();

    player->setVolume(2);
    player->addVoiceSource(0);

    std::cout << "------------------------------------------------------------------------------------------------------\n";
    std::cout << "Available recording devices:\n";
    for (const std::string& deviceName: *recordingDeviceNames)
    {
        std::cout << "  " << deviceName << "\n";
    }

    std::cout << "------------------------------------------------------------------------------------------------------\n";
    std::cout << "Available playback devices:\n";
    for (const std::string& deviceName : *playbackDeviceNames)
    {
        std::cout << "  " << deviceName << "\n";
    }
    std::cout << "------------------------------------------------------------------------------------------------------\n";

    std::cout << "Starting mic loopback. You can try to talk now...\n";
    recorder->startRecording();
    player->startPlaying();

    std::thread readSampleFunctionTask(readSampleFunction, recorder, player);

    std::cout << "Press Enter to stop...\n";
    std::cin.get();

    running = false;

    if (readSampleFunctionTask.joinable()) {
        readSampleFunctionTask.join();
    }

    recorder->stopRecording();
    player->stopPlaying();

    return 0;
}