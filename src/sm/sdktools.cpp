#include "extension.h"
#include "sdktools.h"

#include "call_helper.h"

#include <stdexcept>
#include <string>

namespace sm {
    namespace sdktools {
        IBinTools *g_pBinTools = nullptr;
        IGameConfig *g_pGameConf = nullptr;

        bool SDK_OnLoad(char *error, size_t maxlength, bool late) {
            char conf_error[255];
            if (!gameconfs->LoadGameConfigFile("sdktools.games", &g_pGameConf, conf_error, sizeof(conf_error)))
                return false;

            sharesys->AddDependency(myself, "bintools.ext", true, true);

            SM_GET_IFACE(BINTOOLS, g_pBinTools);

            return true;
        }

        int FindOffset(const char *name)
        {
            int offset;
            if(!g_pGameConf->GetOffset(name, &offset))
                throw std::runtime_error("hook : sig not found - " + std::string(name));
            return offset;
        }

        void SDK_OnUnload() {
            gameconfs->CloseGameConfigFile(g_pGameConf);
        }

        void RemovePlayerItem(CBasePlayer * player, CBaseCombatWeapon *pItem)
        {
            static VFuncCaller<void(CBasePlayer::*)(CBaseCombatWeapon *)> caller(g_pBinTools, FindOffset("RemovePlayerItem"));
            return caller(player, pItem);
        }

        // _ZN11CBasePlayer13GiveNamedItemEPKciP13CEconItemViewb
        // CBaseEntity *CBasePlayer::GiveNamedItem(const char *pszName, int iSubType, CEconItemView *, bool removeIfNotCarried)
        CBaseEntity *GivePlayerItem(CBasePlayer * player, const char *item, int iSubType)
        {
            // TESTED 2020/2/28
            static VFuncCaller<CBaseEntity *(CBasePlayer::*)(const char *, int , CEconItemView *, bool, CBaseEntity *)> caller(g_pBinTools, FindOffset("GiveNamedItem"));
            return caller(player, item, iSubType, nullptr, true, nullptr);
        }

        void SetLightStyle(int style, const char *value)
        {
            return engine->LightStyle(style, value);
        }

        int GivePlayerAmmo(CBasePlayer * player, int amount, int ammotype, bool suppressSound)
        {
            static VFuncCaller<int(CBasePlayer::*)(int, int, bool)> caller(g_pBinTools, FindOffset("GivePlayerAmmo"));
            return caller(player, amount, ammotype, suppressSound);
        }

        void SetEntityModel(CBaseEntity *entity, const char *model)
        {
            static VFuncCaller<void(CBasePlayer::*)(const char *)> caller(g_pBinTools, FindOffset("SetEntityModel"));
            return caller(entity, model);

        }
    }
}