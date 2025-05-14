#include "utils/Helper.hpp"

#include <map>
#include <string>

#include "miniaudio.h"

namespace utils
{
    int getTotalBytes(int sampleRate, int frameSizeMs, int channels, int bytesPerSample)
    {
        return sampleRate * frameSizeMs / 1000 * channels * bytesPerSample;
    }

    std::string maResultToString(ma_result result) {
        static const std::map<ma_result, std::string> ma_result_strings = {
            {MA_SUCCESS, "MA_SUCCESS"},
            {MA_ERROR, "MA_ERROR"},
            {MA_INVALID_ARGS, "MA_INVALID_ARGS"},
            {MA_INVALID_OPERATION, "MA_INVALID_OPERATION"},
            {MA_OUT_OF_MEMORY, "MA_OUT_OF_MEMORY"},
            {MA_OUT_OF_RANGE, "MA_OUT_OF_RANGE"},
            {MA_ACCESS_DENIED, "MA_ACCESS_DENIED"},
            {MA_DOES_NOT_EXIST, "MA_DOES_NOT_EXIST"},
            {MA_ALREADY_EXISTS, "MA_ALREADY_EXISTS"},
            {MA_TOO_MANY_OPEN_FILES, "MA_TOO_MANY_OPEN_FILES"},
            {MA_INVALID_FILE, "MA_INVALID_FILE"},
            {MA_TOO_BIG, "MA_TOO_BIG"},
            {MA_PATH_TOO_LONG, "MA_PATH_TOO_LONG"},
            {MA_NAME_TOO_LONG, "MA_NAME_TOO_LONG"},
            {MA_NOT_DIRECTORY, "MA_NOT_DIRECTORY"},
            {MA_IS_DIRECTORY, "MA_IS_DIRECTORY"},
            {MA_DIRECTORY_NOT_EMPTY, "MA_DIRECTORY_NOT_EMPTY"},
            {MA_AT_END, "MA_AT_END"},
            {MA_NO_SPACE, "MA_NO_SPACE"},
            {MA_BUSY, "MA_BUSY"},
            {MA_IO_ERROR, "MA_IO_ERROR"},
            {MA_INTERRUPT, "MA_INTERRUPT"},
            {MA_UNAVAILABLE, "MA_UNAVAILABLE"},
            {MA_ALREADY_IN_USE, "MA_ALREADY_IN_USE"},
            {MA_BAD_ADDRESS, "MA_BAD_ADDRESS"},
            {MA_BAD_SEEK, "MA_BAD_SEEK"},
            {MA_BAD_PIPE, "MA_BAD_PIPE"},
            {MA_DEADLOCK, "MA_DEADLOCK"},
            {MA_TOO_MANY_LINKS, "MA_TOO_MANY_LINKS"},
            {MA_NOT_IMPLEMENTED, "MA_NOT_IMPLEMENTED"},
            {MA_NO_MESSAGE, "MA_NO_MESSAGE"},
            {MA_BAD_MESSAGE, "MA_BAD_MESSAGE"},
            {MA_NO_DATA_AVAILABLE, "MA_NO_DATA_AVAILABLE"},
            {MA_INVALID_DATA, "MA_INVALID_DATA"},
            {MA_TIMEOUT, "MA_TIMEOUT"},
            {MA_NO_NETWORK, "MA_NO_NETWORK"},
            {MA_NOT_UNIQUE, "MA_NOT_UNIQUE"},
            {MA_NOT_SOCKET, "MA_NOT_SOCKET"},
            {MA_NO_ADDRESS, "MA_NO_ADDRESS"},
            {MA_BAD_PROTOCOL, "MA_BAD_PROTOCOL"},
            {MA_PROTOCOL_UNAVAILABLE, "MA_PROTOCOL_UNAVAILABLE"},
            {MA_PROTOCOL_NOT_SUPPORTED, "MA_PROTOCOL_NOT_SUPPORTED"},
            {MA_PROTOCOL_FAMILY_NOT_SUPPORTED, "MA_PROTOCOL_FAMILY_NOT_SUPPORTED"},
            {MA_ADDRESS_FAMILY_NOT_SUPPORTED, "MA_ADDRESS_FAMILY_NOT_SUPPORTED"},
            {MA_SOCKET_NOT_SUPPORTED, "MA_SOCKET_NOT_SUPPORTED"},
            {MA_CONNECTION_RESET, "MA_CONNECTION_RESET"},
            {MA_ALREADY_CONNECTED, "MA_ALREADY_CONNECTED"},
            {MA_NOT_CONNECTED, "MA_NOT_CONNECTED"},
            {MA_CONNECTION_REFUSED, "MA_CONNECTION_REFUSED"},
            {MA_NO_HOST, "MA_NO_HOST"},
            {MA_IN_PROGRESS, "MA_IN_PROGRESS"},
            {MA_CANCELLED, "MA_CANCELLED"},
            {MA_MEMORY_ALREADY_MAPPED, "MA_MEMORY_ALREADY_MAPPED"},
            {MA_CRC_MISMATCH, "MA_CRC_MISMATCH"},
            {MA_FORMAT_NOT_SUPPORTED, "MA_FORMAT_NOT_SUPPORTED"},
            {MA_DEVICE_TYPE_NOT_SUPPORTED, "MA_DEVICE_TYPE_NOT_SUPPORTED"},
            {MA_SHARE_MODE_NOT_SUPPORTED, "MA_SHARE_MODE_NOT_SUPPORTED"},
            {MA_NO_BACKEND, "MA_NO_BACKEND"},
            {MA_NO_DEVICE, "MA_NO_DEVICE"},
            {MA_API_NOT_FOUND, "MA_API_NOT_FOUND"},
            {MA_INVALID_DEVICE_CONFIG, "MA_INVALID_DEVICE_CONFIG"},
            {MA_LOOP, "MA_LOOP"},
            {MA_BACKEND_NOT_ENABLED, "MA_BACKEND_NOT_ENABLED"},
            {MA_DEVICE_NOT_INITIALIZED, "MA_DEVICE_NOT_INITIALIZED"},
            {MA_DEVICE_ALREADY_INITIALIZED, "MA_DEVICE_ALREADY_INITIALIZED"},
            {MA_DEVICE_NOT_STARTED, "MA_DEVICE_NOT_STARTED"},
            {MA_DEVICE_NOT_STOPPED, "MA_DEVICE_NOT_STOPPED"},
            {MA_FAILED_TO_INIT_BACKEND, "MA_FAILED_TO_INIT_BACKEND"},
            {MA_FAILED_TO_OPEN_BACKEND_DEVICE, "MA_FAILED_TO_OPEN_BACKEND_DEVICE"},
            {MA_FAILED_TO_START_BACKEND_DEVICE, "MA_FAILED_TO_START_BACKEND_DEVICE"},
            {MA_FAILED_TO_STOP_BACKEND_DEVICE, "MA_FAILED_TO_STOP_BACKEND_DEVICE"}
        };

        if (const auto it = ma_result_strings.find(result); it != ma_result_strings.end()) {
            return it->second;
        }

        return "Unknown(" + std::to_string(result) + ")";
    }
}