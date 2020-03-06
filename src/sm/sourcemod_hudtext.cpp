//
// Created by 梅峰源 on 2020-03-05.
//

#include "extension.h"
#include "sourcemod_hudtext.h"

#if SOURCE_ENGINE == SE_CSGO
// fix marco conflict from basetypes.h
#undef schema
#include <cstrike15_usermessages.pb.h>
#endif

namespace sm{
    inline namespace sourcemod {
        inline namespace hudtext {
            extern int g_HudMsgNum;

            void UTIL_SendHudText(int client, const hud_text_parms &textparms, const char *pMessage) {
                cell_t players[1];

                players[0] = client;

#if SOURCE_ENGINE == SE_CSGO
                CCSUsrMsg_HudMsg *msg = (CCSUsrMsg_HudMsg *) usermsgs->StartProtobufMessage(g_HudMsgNum, players, 1, 0);
                msg->set_channel(textparms.channel & 0xFF);

                CMsgVector2D *pos = msg->mutable_pos();
                pos->set_x(textparms.x);
                pos->set_y(textparms.y);

                CMsgRGBA *color1 = msg->mutable_clr1();
                color1->set_r(textparms.r1);
                color1->set_g(textparms.g1);
                color1->set_b(textparms.b1);
                color1->set_a(textparms.a1);

                CMsgRGBA *color2 = msg->mutable_clr2();
                color2->set_r(textparms.r2);
                color2->set_g(textparms.g2);
                color2->set_b(textparms.b2);
                color2->set_a(textparms.a2);

                msg->set_effect(textparms.effect);
                msg->set_fade_in_time(textparms.fadeinTime);
                msg->set_fade_out_time(textparms.fadeoutTime);
                msg->set_hold_time(textparms.holdTime);
                msg->set_fx_time(textparms.fxTime);
                msg->set_text(pMessage);
#else
                bf_write *bf = usermsgs->StartBitBufMessage(g_HudMsgNum, players, 1, 0);
                bf->WriteByte(textparms.channel & 0xFF );
                bf->WriteFloat(textparms.x);
                bf->WriteFloat(textparms.y);
                bf->WriteByte(textparms.r1);
                bf->WriteByte(textparms.g1);
                bf->WriteByte(textparms.b1);
                bf->WriteByte(textparms.a1);
                bf->WriteByte(textparms.r2);
                bf->WriteByte(textparms.g2);
                bf->WriteByte(textparms.b2);
                bf->WriteByte(textparms.a2);
                bf->WriteByte(textparms.effect);
                bf->WriteFloat(textparms.fadeinTime);
                bf->WriteFloat(textparms.fadeoutTime);
                bf->WriteFloat(textparms.holdTime);
                bf->WriteFloat(textparms.fxTime);
                bf->WriteString(pMessage);
#endif
                usermsgs->EndMessage();
            }

        }
    }
}