#include "win32_ss.h"
#include "ss_tools.h"

internal void
Win32GetEXEFileName(Win32State *state)
{
    // NOTE: Never use MAX_PATH in code that is user-facing, because it
    // can be dangerous and lead to bad results.
    DWORD sizeOfFilename = GetModuleFileNameA(0, state->EXEFileName, sizeof(state->EXEFileName));
    state->OnePastLastEXEFileNameSlash = state->EXEFileName;
    for (char *scan = state->EXEFileName; *scan; ++scan)
    {
        if (*scan == '\\')
        {
            state->OnePastLastEXEFileNameSlash = scan + 1;
        }
    }
}

internal void
Win32BuildEXEPathFileName(Win32State *state, char *fileName, int destCount, char *dest)
{
    CatStrings(state->OnePastLastEXEFileNameSlash - state->EXEFileName, state->EXEFileName,
               StringLength(fileName), fileName, destCount, dest);
}

DEBUG_PLATFORM_FREE_FILE_MEMORY(DEBUGPlatformFreeFileMemory)
{
    if (memory)
    {
        VirtualFree(memory, 0, MEM_RELEASE);
    }
}

DEBUG_PLATFORM_READ_ENTIRE_FILE(DEBUGPlatformReadEntireFile)
{
    DebugReadFileResult result = {};

    auto fileHandle = CreateFileA(filename, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
    if (fileHandle != INVALID_HANDLE_VALUE)
    {
        LARGE_INTEGER fileSize;
        if (GetFileSizeEx(fileHandle, &fileSize))
        {
            u32 fileSize32 = TruncateU64(fileSize.QuadPart);
            result.Contents = VirtualAlloc(0, fileSize32, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
            if (result.Contents)
            {
                DWORD bytesRead;
                if (ReadFile(fileHandle, result.Contents, fileSize32, &bytesRead, 0) &&
                    (fileSize32 == bytesRead))
                {
                    // NOTE: File read successfully
                    result.ContentsSize = fileSize32;
                }
                else
                {
                    // TODO: Logging
                    DEBUGPlatformFreeFileMemory(thread, result.Contents);
                    result.Contents = 0;
                }
            }
            else
            {
                // TODO: Logging
            }
        }
        else
        {
            // TODO: Logging
        }

        CloseHandle(fileHandle);
    }
    else
    {
        // TODO: Logging
    }

    return result;
}

DEBUG_PLATFORM_WRITE_ENTIRE_FILE(DEBUGPlatformWriteEntireFile)
{
    b32 result = false;

    auto fileHandle = CreateFileA(filename, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, 0, 0);
    if (fileHandle != INVALID_HANDLE_VALUE)
    {
        DWORD bytesWritten;
        if (WriteFile(fileHandle, memory, memorySize, &bytesWritten, 0))
        {
            // NOTE: File read successfully
            result = (bytesWritten == memorySize);
        }
        else
        {
            // TODO: Logging
        }

        CloseHandle(fileHandle);
    }
    else
    {
        // TODO: Logging
    }

    return result;
}

inline FILETIME
Win32GetLastWriteTime(char *filename)
{
    FILETIME lastWriteTime = {};

    WIN32_FILE_ATTRIBUTE_DATA data;
    if (GetFileAttributesEx(filename, GetFileExInfoStandard, &data))
    {
        lastWriteTime = data.ftLastWriteTime;
    }

    return lastWriteTime;
}

internal Win32AppCode
Win32LoadAppCode(char *sourceDLLName, char *tempDLLName)
{
    Win32AppCode result = {};

    // TODO: Need to get the proper path here!
    // TODO: Automatic determination of when updates are necessary.

    result.DLLLastWriteTime = Win32GetLastWriteTime(sourceDLLName);
    CopyFile(sourceDLLName, tempDLLName, FALSE);

    result.AppCodeDLL = LoadLibraryA(tempDLLName);
    if (result.AppCodeDLL)
    {
        result.Calculate = (CalculateType *)
            GetProcAddress(result.AppCodeDLL, "Calculate");

        result.IsValid = (result.Calculate != 0);
    }

    if (!result.IsValid)
    {
        result.Calculate = CalculateStub;
    }

    return result;
}

internal void
Win32UnloadAppCode(Win32AppCode *appCode)
{
    if (appCode->AppCodeDLL)
    {
        FreeLibrary(appCode->AppCodeDLL);
        appCode->AppCodeDLL = 0;
    }

    appCode->IsValid = false;
    appCode->Calculate = CalculateStub;
}

int main()
{
    Win32State win32State = {};
    Win32GetEXEFileName(&win32State);

    char sourceAppCodeDLLFullPath[WIN32_STATE_FILE_NAME_COUNT];
    Win32BuildEXEPathFileName(&win32State, "ss.dll",
                              sizeof(sourceAppCodeDLLFullPath), sourceAppCodeDLLFullPath);

    char tempAppCodeDLLFullPath[WIN32_STATE_FILE_NAME_COUNT];
    Win32BuildEXEPathFileName(&win32State, "ss_temp.dll",
                              sizeof(tempAppCodeDLLFullPath), tempAppCodeDLLFullPath);

    auto app = Win32LoadAppCode(sourceAppCodeDLLFullPath, tempAppCodeDLLFullPath);

    app.Calculate();
    return 0;
}
