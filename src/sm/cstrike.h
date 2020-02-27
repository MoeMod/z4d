#pragma once

#include "hook_types.h"
#include "cstrike/csgo_const.h"

namespace sm {
    namespace cstrike {

        bool SDK_OnLoad(char* error, size_t maxlength, bool late);
        void SDK_OnUnload();

        void TerminateRound(CGameRules *gamerules, float delay, CSRoundEndReason_e reason);
        void CS_TerminateRound(float delay, CSRoundEndReason_e reason);


        void SwitchTeam(CGameRules *gamerules, CBaseEntity *pEntity, CSTeam_e iTeam);
        void CS_SwitchTeam(CBaseEntity *pEntity, CSTeam_e iTeam);

        void CS_RespawnPlayer(CBaseEntity *pEntity);
        void CS_UpdateClientModel(CBaseEntity *pEntity);

        void CS_DropWeapon(CBaseEntity *pEntity, CBaseCombatWeapon *pWeapon, bool toss);
    }
}
