#pragma once

#include <cstdio>
#include <math.h>
#include <stdint.h>

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef i32 b32;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef float f32;
typedef double f64;

struct ThreadContext
{
    int Placeholder;
};

#if EDITOR_INTERNAL

struct DebugReadFileResult
{
    u32 ContentsSize;
    void *Contents;
};

#define DEBUG_PLATFORM_FREE_FILE_MEMORY(name) void name(ThreadContext *thread, void *memory)
typedef DEBUG_PLATFORM_FREE_FILE_MEMORY(DebugPlatformFreeFileMemoryType);

#define DEBUG_PLATFORM_READ_ENTIRE_FILE(name) DebugReadFileResult name(ThreadContext *thread, char *filename)
typedef DEBUG_PLATFORM_READ_ENTIRE_FILE(DebugPlatformReadEntireFileType);

#define DEBUG_PLATFORM_WRITE_ENTIRE_FILE(name) b32 name(ThreadContext *thread, char *filename, u32 memorySize, void *memory)
typedef DEBUG_PLATFORM_WRITE_ENTIRE_FILE(DebugPlatformWriteEntireFileType);

#endif

#define CALCULATE(name) void name()
typedef CALCULATE(CalculateType);
CALCULATE(CalculateStub)
{
}
