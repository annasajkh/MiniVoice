#include "utils/Helper.hpp"

namespace utils
{
    int getTotalBytes(int sampleRate, int frameSizeMs, int channels, int bytesPerSample)
    {
        return sampleRate * frameSizeMs / 1000 * channels * bytesPerSample;
    }
}