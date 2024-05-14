#pragma once

#include "Sfix.hpp"

using namespace RC::Unreal;

struct FActionParameter;

namespace nBattle {
    // The generic battle object type.

    class cWork {
    public:
        struct RANGE {
            uint16 U; // 0x2
            uint16 D; // 0x4
            uint16 L; // 0x6
            uint16 R; // 0x8
            uint16 unk1; // 0xa
        }; // size: 0xc

        bool be_flag; // 0x8
        uint8 pl_type; // 0x9
        uint8 pl_no; // 0xa
        uint8 pl_work_no; // 0xb
        uint8 unk0[0x14]; // 0xc
        cWork* work_prev; // 0x20
        cWork* work_next; // 0x28
        uint64 unk1[4]; // 0x30
        cWork* work_address; // 0x50
        cWork* master_add; // 0x58
        cWork* owner_add; // 0x60
        uint64 unk2[3]; // 0x68

        sfix3 pos; // 0x80 
        sfix3 pos_old; // 0x90
        sfix3 speed; // 0xa0
        sfix3 alpha; // 0xb0
        uint16 BitValue; // 0xc0
        uint8 ignore_steer; // 0xc2
        uint8 shl_flags; // 0xc3
        bool Dir; // 0xc4
        bool unk3; // 0xc5
        RANGE Range; // 0xc6

        uint8 temp_pad[0x110];

        cWork();
        virtual ~cWork() = default;

        FIELD(0x160, FActionParameter*, mpActParam);
        FIELD(0xF, uint8, status);
    }; // size: 0x1e0
}
