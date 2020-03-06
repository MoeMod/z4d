#pragma once

namespace sm{
    inline namespace sourcemod {
        inline namespace players {
            // core/logic/smn_players.cpp
            void ChangeClientTeam(IGamePlayer* pPlayer, int team);
            bool IsClientConnected(IGamePlayer* pPlayer);
            bool IsPlayerAlive(IGamePlayer* pPlayer);
            bool IsPlayerAlive(CBaseEntity* pEntity);

            int GetHealth(IGamePlayer* pPlayer);
            int GetArmorValue(IGamePlayer* pPlayer);

            const char* GetClientName(IGamePlayer* pPlayer);
        }
    }
}