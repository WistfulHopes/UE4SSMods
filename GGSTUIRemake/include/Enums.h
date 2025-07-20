#pragma once
#include "HAL/Platform.hpp"

using namespace RC::Unreal;

enum SIDE_ID
{
    SIDE_BEGIN = 0x0,
    SIDE_1P = SIDE_BEGIN,
    SIDE_2P = 1,
    SIDE_ID_NUM = 2,
    SIDE_COMMON = SIDE_ID_NUM,
    SIDE_ID_NUM_WITH_COMMON = 3,
    SIDE_ID_INVALID = 4,
};

enum PAD_ID
{
    PAD_ID_BEGIN = 0,
    PAD_ID_1CON = PAD_ID_BEGIN,
    PAD_ID_2CON,
    PAD_ID_3CON,
    PAD_ID_4CON,
    PAD_ID_MAX,
    PAD_ID_CPU = PAD_ID_MAX,
    PAD_ID_MAX_WITH_CPU,
    PAD_ID_ANY,
    PAD_ID_EX1,
    PAD_ID_EX2,
    PAD_ID_INVALID,
};
