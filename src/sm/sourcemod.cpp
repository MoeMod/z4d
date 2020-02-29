#include "extension.h"
#include "sourcemod.h"

#include <game/server/iplayerinfo.h>
#include <PlayerManager.h>

namespace sm {
    inline namespace sourcemod {

        void ChangeClientTeam(IGamePlayer *pPlayer, int team) {
            IPlayerInfo *pInfo = pPlayer->GetPlayerInfo();
            return pInfo->ChangeTeam(team);
        }

        bool IsPlayerAlive(CBasePlayer *player) {
            return GetEntProp(player, Prop_Data, "m_lifeState") == PLAYER_LIFE_ALIVE;
        }

        bool IsClientConnected(IGamePlayer *pPlayer) {
            return pPlayer && pPlayer->IsConnected();
        }

        int GetArmorValue(IGamePlayer *pPlayer) {
            return pPlayer->GetPlayerInfo()->GetArmorValue();
        }

        int GetHealth(IGamePlayer *pPlayer) {
            return pPlayer->GetPlayerInfo()->GetHealth();
        }

    }
}

