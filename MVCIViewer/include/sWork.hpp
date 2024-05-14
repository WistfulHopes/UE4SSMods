#pragma once

#include "cWork.hpp"

namespace nBattle { 
    // Storage struct for cWork instances.

    struct sWork {
        struct UnkData {
            int unk0;
            int unk1;
            int unk2;
        }; // size: 0xc

        void* Pull_work_jmp; // 0x8
        cWork Global_work[0x100]; // 0x10
        UnkData unk0[0x100]; // 0x10

        sWork();
        virtual ~sWork() = default;
    }; // size: 0x1e5b0
}
