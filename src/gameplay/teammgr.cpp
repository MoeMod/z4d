#include "extension.h"
#include "teammgr.h"
#include <stdint.h>
#include "cstrike/csgo_const.h"
#include "sm/sourcemod.h"
#include "sm/cstrike.h"
#include "tools.h"

#include <array>
#include <bitset>
#include <numeric>

namespace gameplay {
    namespace teammgr {
        void Init() {

        }

        void ApplyOnClientTeam(CBaseEntity *client, CSTeam_e iTeam) {
            // Switch team
            bool bState = GetDefuser(client);
            SetTeam(client, iTeam);
            SetDefuser(client, bState); /// HACK~HACK

            // Sets glowing for the zombie vision
            //ToolsSetDetecting(client, ModesIsXRay(gServerData.RoundMode));
        }

        void Team_Set(int id, Team_e team) {
            ApplyOnClientTeam(sm::CBaseEntityFrom(id), (CSTeam_e)team);
        }

        Team_e Team_Get(int id) {
            return (Team_e)GetTeam(sm::CBaseEntityFrom(id));
        }

        int TeamCount(Team_e team, bool AliveOnly) {
            std::array<int, SM_MAXPLAYERS> ids;
            std::iota(ids.begin(), ids.end(), 1);
            return std::count_if(ids.begin(), ids.end(), [AliveOnly, team](int id){return (AliveOnly ? IsAlive : IsConnected)(id) && Team_Get(id) == team; });
        }
    }
}