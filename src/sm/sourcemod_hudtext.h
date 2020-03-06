#pragma once

namespace sm{
    inline namespace sourcemod {
        inline namespace hudtext {

            struct hud_text_parms {
                float x;
                float y;
                int effect;
                byte r1, g1, b1, a1;
                byte r2, g2, b2, a2;
                float fadeinTime;
                float fadeoutTime;
                float holdTime;
                float fxTime;
                int channel;
            };

            void UTIL_SendHudText(int client, const hud_text_parms &textparms, const char *pMessage);

        }
    }
}