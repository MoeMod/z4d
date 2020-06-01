#pragma once

#include <shared/shareddefs.h>

namespace sm{
    inline namespace sourcemod {
        inline namespace halflife {

            extern int g_SayTextMsg;

            inline float GetGameTime() {
                return gpGlobals->curtime;
            }

            inline bool PrintToChat(int id, const char *buffer) {
                g_pSM->SetGlobalTarget(id);
                return gamehelpers->TextMsg(id, HUD_PRINTTALK, buffer);
            }

            inline bool PrintToChatStr(int id, std::string str) {
                return PrintToChat(id, str.c_str());
            }

            inline void PrintToChatAll(const char *buffer) {
                for (int i = 1; i <= playerhelpers->GetMaxClients(); i++)
                {
                    if (IsClientInGame(IGamePlayerFrom(i)))
                    {
                        g_pSM->SetGlobalTarget(i);
                        PrintToChat(i, buffer);
                    }
                }
            }

            inline void PrintToChatAllStr(std::string buffer) {
                return PrintToChatAll(buffer.c_str());
            }
        }
    }
}