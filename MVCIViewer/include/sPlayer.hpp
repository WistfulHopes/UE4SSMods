#pragma once

#include "cPlayer.hpp"

namespace nBattle {
    // Storage struct for cPlayer instances.

    struct sPlayer {
        uint8 prev_no_push_bit; // 0x8
        uint8 body_hosei_flag; // 0x9
        uint8 pl_move_limit; // 0xa
        uint8 fusion_flag; // 0xb
        int32 move_ctr; // 0xc
        cPlayer mcPlayer[6]; // 0x10
        cPlayer mcPlayerDummy[6]; // 0x2ad0
        cPlayer* mpChainTop; // 0x5590
        cPlayer* mpChainEnd; // 0x5598
        cPlayer* mpHoldTop; // 0x55A0
        cPlayer* mpHoldEnd; // 0x55A8
        uint32 mPlayerType[6]; // 0x55B0
        uint32 mPlayerTypeDummy[6]; // 0x55B0

        sPlayer();
        virtual ~sPlayer() = default;
    }; // size: 0x6e10
};