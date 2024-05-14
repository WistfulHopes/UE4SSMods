#pragma once

#include "HAL/Platform.hpp"

using namespace RC::Unreal;

typedef int32 sfix;

struct sfix2 {
    sfix x;
    sfix y;
};

struct alignas(8) sfix3 {
    sfix x;
    sfix y;
    sfix z;
};

struct sfix4 {
    sfix x;
    sfix y;
    sfix z;
    sfix w;
};