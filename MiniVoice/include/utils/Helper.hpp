#pragma once
#include <string>

#include "miniaudio.h"
#include "../MiniVoiceExport.hpp"

namespace utils {
    int MINIVOICE_API getTotalBytes(int sampleRate, int frameSizeMs, int channels, int bytesPerSample);
    std::string maResultToString(ma_result result);
}