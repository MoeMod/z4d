#pragma once

#include "hook_types.h"
#include "csgo_const.h"

namespace hook {
    inline namespace cstrike {

        bool CS_SDK_OnLoad(char* error, size_t maxlength, bool late);
        void CS_SDK_OnAllLoaded();
        void CS_SDK_OnUnload();

        void TerminateRound(CGameRules *gamerules, float delay, CSRoundEndReason_e reason);
        void CS_TerminateRound(float delay, CSRoundEndReason_e reason);


        void SwitchTeam(CGameRules *gamerules, CBaseEntity *pEntity, CSTeam_e iTeam);
        void CS_SwitchTeam(CBaseEntity *pEntity, CSTeam_e iTeam);

        void CS_RespawnPlayer(CBaseEntity *pEntity);
        void CS_UpdateClientModel(CBaseEntity *pEntity);

        void CS_DropWeapon(CBaseEntity *pEntity, CBaseCombatWeapon *pWeapon, bool toss);
    }
}
