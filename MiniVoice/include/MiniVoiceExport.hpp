#pragma once

#if defined(_MSC_VER)
    #if defined(MINIVOICE_EXPORTS)
        #define MINIVOICE_API __declspec(dllexport)
    #else
        #define MINIVOICE_API __declspec(dllimport)
    #endif
#else
    #if defined(MINIVOICE_EXPORTS)
        #define MINIVOICE_API __attribute__((visibility("default")))
    #else
        #define MINIVOICE_API
    #endif
#endif