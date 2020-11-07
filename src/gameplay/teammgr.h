#pragma once

#include "sm/sourcemod_types.h"
#include "sm/cstrike_const.h"
#include "tools.h"

namespace gameplay {
    namespace teammgr {
        
        constexpr auto ZB_TEAM_ZOMBIE = CS_TEAM_T;
        constexpr auto ZB_TEAM_HUMAN = CS_TEAM_CT;

        inline bool IsConnected(int id) {
            auto p = sm::IGamePlayerFrom(id); // nullable
            return p && p->IsConnected();
        }

        inline bool IsAlive(int id) {
            auto p = sm::IGamePlayerFrom(id); // nullable
            return sm::IsClientConnected(p) && sm::IsPlayerAlive(p);
        }

        void Init();
        CSTeam_e Team_Get(int id);
        void Team_Set(int id, CSTeam_e team);
        int TeamCount(CSTeam_e team, bool AliveOnly = false);

        bool IsConnected(int id);
        bool IsAlive(int id);
    }
}