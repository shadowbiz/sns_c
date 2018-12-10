#pragma once

#include "ss.h"

#include <windows.h>
#include <initguid.h>
#include <avrt.h>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#define WIN32_STATE_FILE_NAME_COUNT MAX_PATH
struct Win32State
{
    u64 TotalSize;
    void *AppMemoryBlock;

    HANDLE RecordingHandle;
    i32 InputRecordingIndex;

    HANDLE PlaybackHandle;
    i32 InputPlayingIndex;

    char EXEFileName[WIN32_STATE_FILE_NAME_COUNT];
    char *OnePastLastEXEFileNameSlash;
};

struct Win32AppCode
{
    HMODULE AppCodeDLL;
    FILETIME DLLLastWriteTime;

    CalculateType *Calculate;

    b32 IsValid;
};