#pragma once

#define internal static
#define local static
#define global static

#include "ss_platform.h"

#if EDITOR_SLOW
#define Assert(expression) \
    if (!(expression))     \
    {                      \
        *(int *)0 = 0;     \
    }
#else
#define Assert(expression)
#endif

#include "ss_math.h"

#define ArrayCount(array) (sizeof(array) / sizeof((array)[0]))

#define Kilobytes(bytes) ((bytes)*1024LL)
#define Megabytes(bytes) (Kilobytes(bytes) * 1024LL)
#define Gigabytes(bytes) (Megabytes(bytes) * 1024LL)
#define Terabytes(bytes) (Gigabytes(bytes) * 1024LL)

#define ToKilobytes(bytes) ((bytes) / 1024LL)
#define ToMegabytes(bytes) (ToKilobytes(bytes) / 1024LL)
#define ToGigabytes(bytes) (ToMegabytes(bytes) / 1024LL)
#define ToTerabytes(bytes) (ToGigabytes(bytes) / 1024LL)

#define MillimeterToMeter(m) ((m) / 1000.0)

struct PipeDiameter
{
    f64 DOut; // внешний диаметр в метрах
    f64 DIn;  // внутренний диаметр в метрах
};

struct FireChamber
{
    f64 TopLengh;
    f64 TopWidth;
    f64 BottomLength;
    f64 BottomWidth;
    f64 FrontHeight;
    f64 RearHeight;

    f64 U;
    f64 R;
    f64 Ht;
};

struct Boiler
{
    f64 Hdg; // поверхность нагрева дымогарных труб (газовая)
    f64 Hz1; // поверхность нагрева жаровых труб (газовая)
    f64 Hz2; // поверхность нагрева жаровых труб (газовая)
    f64 Hk;  // полная испаряющая поверхность нагрева котла (водяная)
    f64 Hi;  // поверхность нагрева пароперегревателя (газовая)
    f64 Ld;  // Длина труб дымогарных
    f64 Lz1; // Длина труб жаровых до перегревателя
    f64 Lz2; // Длина труб жаровых в области перегревателя

    u16 Nd; // число дымогарных труб
    u16 Nz; // число жаровых труб

    PipeDiameter Dd;  // диаметр труб дымогарных
    PipeDiameter Dz1; // диаметр труб жаровых до перегревателя
    PipeDiameter Dz2; // диаметр труб жаровых в области перегревателя
    PipeDiameter Di;  // диаметр труб перегревательных
};

struct Fuel
{
    f64 C;
    f64 H;
    f64 S;
    f64 O;
    f64 N;
    f64 W;
    f64 A;
    f64 CO;
    f64 CO2;
    f64 O2;
    f64 N2;
    f64 K;     // теплопроизводительность 1 кг топлива (низшая)
    f64 beta0; // химическая характеристика топлива
    f64 alpha; // коэффициент избытка топлива
    f64 u;
};

struct HeatCoefficient
{
    f64 M;
    f64 N;
};
