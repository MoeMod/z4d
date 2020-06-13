#include "extension.h"
#include "teammgr.h"
#include <stdint.h>
#include "sm/sourcemod.h"
#include "sm/cstrike.h"
#include "tools.h"
#include "gametypes.h"

#include <array>
#include <bitset>
#include <numeric>
#include <random>

namespace gameplay {
    namespace teammgr {
        void Init() {

        }

        int NumTModels()
        {
            const char* map = sm::GetCurrentMapSz();
            auto pTModelNames = gametypes::g_pGameTypes->GetTModelsForMap(map);
            return pTModelNames ? pTModelNames->Count() : 1;
        }

        int NumCTModels()
        {
            const char* map = sm::GetCurrentMapSz();
            auto pCTModelNames = gametypes::g_pGameTypes->GetCTModelsForMap(map);
            return pCTModelNames ? pCTModelNames->Count() : 1;
        }

        int GetNextClassForTeam(CSTeam_e team)
        {
            static std::random_device rd;
            if (team == CS_TEAM_T)
                return std::uniform_int_distribution<int>(GetFirstTClass(), GetLastTClass())(rd);
            else if (team == CS_TEAM_CT)
                return std::uniform_int_distribution<int>(GetFirstCTClass(), GetLastCTClass())(rd);
            return 0;
        }

        void ApplyOnClientTeam(CBaseEntity *client, CSTeam_e iTeam) {
            // Switch team
            bool bState = GetDefuser(client);
            SetTeam(client, iTeam);
            SetDefuser(client, bState); /// HACK~HACK

            // Sets glowing for the zombie vision
            //ToolsSetDetecting(client, ModesIsXRay(gServerData.RoundMode));

            // Hack! no more crash on round start
            sm::SetEntProp(client, sm::Prop_Send, "m_iClass", CS_CLASS_NONE);
        }

        void Team_Set(int id, CSTeam_e team) {
            ApplyOnClientTeam(sm::CBaseEntityFrom(id), (CSTeam_e)team);
        }

        CSTeam_e Team_Get(int id) {
            auto cbase = sm::CBaseEntityFrom(id);
            return cbase ? (CSTeam_e)GetTeam(cbase) : CS_TEAM_NONE;
        }

        int TeamCount(CSTeam_e team, bool AliveOnly) {
            std::array<int, SM_MAXPLAYERS> ids;
            std::iota(ids.begin(), ids.end(), 1);
            return std::count_if(ids.begin(), ids.end(), [AliveOnly, team](int id){return (AliveOnly ? IsAlive : IsConnected)(id) && Team_Get(id) == team; });
        }
    }
}