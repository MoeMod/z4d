#pragma once

#include "sm/hook_types.h"
#include "cstrike/csgo_const.h"
#include "tools.h"

namespace gameplay {
    namespace teammgr {
        enum Team_e {
            Z4D_TEAM_NONE = CS_TEAM_NONE,
            Z4D_TEAM_SPECTATOR = CS_TEAM_SPECTATOR,
            Z4D_TEAM_ZOMBIE = CS_TEAM_T,
            Z4D_TEAM_HUMAN = CS_TEAM_CT,
        };

        inline bool IsConnected(int id) {
            auto p = playerhelpers->GetGamePlayer(id); // nullable
            return p && p->IsConnected();
        }

        inline bool IsAlive(int id) {
            return IsConnected(id) && sm::IsPlayerAlive(sm::CBaseEntityFrom(id));
        }

        void Init();
        Team_e Team_Get(int id);
        void Team_Set(int id, Team_e team);
        int TeamCount(Team_e team, bool AliveOnly = false);

        bool IsConnected(int id);
        bool IsAlive(int id);
    }
}