#pragma once

#include "cWork.hpp"
#include "HAL/Platform.hpp"

namespace nBattle {
    // The generic player object type. Inherits from cWork.

    class cPlayer : public cWork {
    public:
        uint8 temp_pad_ply[0x540];

        FIELD(0x265, uint8, hitstun_scale);

        cPlayer();
        virtual ~cPlayer() = default;
    }; // size: 0x720
}
