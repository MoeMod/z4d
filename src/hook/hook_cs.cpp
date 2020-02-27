
#include "extension.h"

#include "hook_cs.h"
#include <sm_argbuffer.h>

#include <extensions/IBinTools.h>
#include <extensions/ISDKHooks.h>
#include <extensions/ISDKTools.h>

#include <string>
#include <stdexcept>

namespace hook {
    inline namespace cstrike {
        IBinTools *g_pBinTools = nullptr;
        ISDKTools *g_pSDKTools = nullptr;
        IGameConfig *g_pGameConf = nullptr;

        void *FindSig(const char *name)
        {
            void *addr;
            if(!g_pGameConf->GetMemSig("TerminateRound", &addr))
                throw std::runtime_error("hook : sig not found - " + std::string(name));
            return addr;
        }

        void TerminateRound(CGameRules *gamerules, float delay, CSRoundEndReason_e reason) {

#ifndef _WIN32
            PassInfo pass[4];
            pass[0].flags = PASSFLAG_BYVAL;
            pass[0].type = PassType_Float;
            pass[0].size = sizeof(float);
            pass[1].flags = PASSFLAG_BYVAL;
            pass[1].type = PassType_Basic;
            pass[1].size = sizeof(int);
            pass[2].flags = PASSFLAG_BYVAL;
            pass[2].type = PassType_Basic;
            pass[2].size = sizeof(int);
            pass[3].flags = PASSFLAG_BYVAL;
            pass[3].type = PassType_Basic;
            pass[3].size = sizeof(int);
            static ICallWrapper *pWrapper = g_pBinTools->CreateCall(FindSig("TerminateRound"), CallConv_ThisCall, NULL, pass, 4);

            ArgBuffer<void*, float, int, int, int> vstk(gamerules, delay, reason, 0, 0);
            pWrapper->Execute(vstk, nullptr);
#else
            static void *addr = FindSig("TerminateRound");
			__asm
			{
			push 0
			push 0
			push reason
			movss xmm1, delay
			mov ecx, gamerules
			call addr
			}
#endif
        }

        void CS_TerminateRound(float delay, CSRoundEndReason_e reason) {
            return TerminateRound(static_cast<CGameRules *>(g_pSDKTools->GetGameRules()), delay, reason);
        }

        void CS_RespawnPlayer(CBaseEntity *pEntity) {
            static auto pWrapper = g_pBinTools->CreateCall(FindSig("RoundRespawn"), CallConv_ThisCall, NULL, NULL, 0);
            pWrapper->Execute(&pEntity, NULL);
        }

        void CS_UpdateClientModel(CBaseEntity *pEntity) {
            static auto pWrapper = g_pBinTools->CreateCall(FindSig("SetModelFromClass"), CallConv_ThisCall, NULL, NULL, 0);
            pWrapper->Execute(&pEntity, NULL);
        }

        void SwitchTeam(CGameRules *gamerules, CBaseEntity *pEntity, CSTeam_e team) {

#ifndef _WIN32
            PassInfo pass[1];
            pass[0].flags = PASSFLAG_BYVAL;
            pass[0].type = PassType_Basic;
            pass[0].size = sizeof(int);
            static ICallWrapper *pWrapper = g_pBinTools->CreateCall(FindSig("SwitchTeam"), CallConv_ThisCall, NULL, pass, 4);

            ArgBuffer<CBaseEntity*, int> vstk(pEntity, team);
            pWrapper->Execute(vstk, nullptr);
#else
            static void *addr = FindSig("SwitchTeam");
			__asm
			{
			push team
			mov ecx, pEntity
			mov ebx, gamerules
			call addr
			}
#endif
        }

        void CS_SwitchTeam(CBaseEntity *pEntity, CSTeam_e team) {
            return SwitchTeam(static_cast<CGameRules *>(g_pSDKTools->GetGameRules()), pEntity, team);
        }

        void CS_DropWeapon(CBaseEntity *pEntity, CBaseCombatWeapon *pWeapon, bool toss) {
            static auto pWrapper = g_pBinTools->CreateCall(FindSig("DropWeaponBB"), CallConv_ThisCall, NULL, NULL, 0);
            ArgBuffer<CBaseEntity*, CBaseCombatWeapon*, bool> vstk(pEntity, pWeapon, toss);
            pWrapper->Execute(vstk, NULL);
        }

        bool CS_SDK_OnLoad(char *error, size_t maxlength, bool late) {
            char conf_error[255];
            if (!gameconfs->LoadGameConfigFile("sm-cstrike.games", &g_pGameConf, conf_error, sizeof(conf_error)))
            {
                if (error)
                {
                    ke::SafeSprintf(error, maxlength, "Could not read sm-cstrike.games: %s", conf_error);
                }
                return false;
            }
        }

        void CS_SDK_OnAllLoaded() {

            SM_GET_LATE_IFACE(BINTOOLS, g_pBinTools);

            SM_GET_LATE_IFACE(SDKTOOLS, g_pSDKTools);

            return void();
        }

        void CS_SDK_OnUnload() {
            gameconfs->CloseGameConfigFile(g_pGameConf);
        }

    }
}