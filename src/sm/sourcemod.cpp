#include "extension.h"
#include "sourcemod.h"

#include "sm/sdkhooks.h"
#include "sm/sdktools.h"
#include "sm/cstrike.h"

#include <game/server/iplayerinfo.h>
#include <PlayerManager.h>

namespace sm {
    inline namespace sourcemod {
        static IGameConfig *g_pGameConf = nullptr;
        static int lifestate_offset = -1;

        int GetLifeState(CBaseEntity *pEntity)
        {
            if (lifestate_offset == -1)
            {
                if (!g_pGameConf->GetOffset("m_lifeState", &lifestate_offset))
                {
                    lifestate_offset = -2;
                }
            }

            if (*((uint8_t *)pEntity + lifestate_offset) == LIFE_ALIVE)
            {
                return PLAYER_LIFE_ALIVE;
            }
            else
            {
                return PLAYER_LIFE_DEAD;
            }
        }

        int GetLifeState(IGamePlayer *player)
        {
            if (lifestate_offset == -1)
            {
                if (!g_pGameConf->GetOffset("m_lifeState", &lifestate_offset))
                {
                    lifestate_offset = -2;
                }
            }

            if (lifestate_offset < 0)
            {
                IPlayerInfo *info =  player->GetPlayerInfo();
                if (info == NULL)
                {
                    return PLAYER_LIFE_UNKNOWN;
                }
                return info->IsDead() ? PLAYER_LIFE_DEAD : PLAYER_LIFE_ALIVE;
            }

            auto edict = player->GetEdict();
            if (edict == NULL)
            {
                return PLAYER_LIFE_UNKNOWN;
            }

            CBaseEntity *pEntity;
            IServerUnknown *pUnknown = edict->GetUnknown();
            if (pUnknown == NULL || (pEntity = pUnknown->GetBaseEntity()) == NULL)
            {
                return PLAYER_LIFE_UNKNOWN;
            }

            return GetLifeState(pEntity);
        }

        void ChangeClientTeam(IGamePlayer *pPlayer, int team) {
            IPlayerInfo *pInfo = pPlayer->GetPlayerInfo();
            return pInfo->ChangeTeam(team);
        }

        bool IsPlayerAlive(IGamePlayer *player) {
            return GetLifeState(player) == PLAYER_LIFE_ALIVE;
        }

        bool IsPlayerAlive(CBaseEntity *pEntity) {
            return GetLifeState(pEntity) == PLAYER_LIFE_ALIVE;
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

        bool SDK_OnLoad(char *error, size_t maxlen, bool late) {
            char conf_error[255];
            if (!gameconfs->LoadGameConfigFile("core.games", &g_pGameConf, conf_error, sizeof(conf_error)))
            {
                if (error)
                {
                    ke::SafeSprintf(error, maxlen, "Could not read core.games: %s", conf_error);
                }
                return false;
            }
            sm::sdkhooks::SDK_OnLoad(error, maxlen, late);
            sm::sdktools::SDK_OnLoad(error, maxlen, late);
            sm::cstrike::SDK_OnLoad(error, maxlen, late);
            return true;
        }

        void SDK_OnUnload() {
            sm::sdkhooks::SDK_OnUnload();
            sm::sdktools::SDK_OnUnload();
            sm::cstrike::SDK_OnUnload();

        }

    }
}

