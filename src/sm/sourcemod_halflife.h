#pragma once

namespace sm{
    inline namespace sourcemod {
        inline namespace halflife {

            inline float GetGameTime() {
                return gpGlobals->curtime;
            }

        }
    }
}