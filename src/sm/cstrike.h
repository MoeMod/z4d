#pragma once

#include "hook_types.h"
#include "hook_result.h"
#include "cstrike_const.h"

namespace sm {
    namespace cstrike {

        bool SDK_OnLoad(char* error, size_t maxlength, bool late);
        void SDK_OnUnload();

        void CS_TerminateRound(float delay, CSRoundEndReason_e reason, bool blockhook = false);

        void SwitchTeam(CGameRules *gamerules, CBaseEntity *pEntity, CSTeam_e iTeam);
        void CS_SwitchTeam(CBaseEntity *pEntity, CSTeam_e iTeam);

        void CS_RespawnPlayer(CBaseEntity *pEntity);
        void CS_UpdateClientModel(CBaseEntity *pEntity);

        void CS_DropWeapon(CBaseEntity *pEntity, CBaseCombatWeapon *pWeapon, bool toss);

        inline namespace forwards {
            extern EventDispatcher<Action(float&, CSRoundEndReason_e&)> CS_OnTerminateRound;
        }
    }
}