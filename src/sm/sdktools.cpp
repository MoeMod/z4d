#include "extension.h"
#include "sdktools.h"

#include <extensions/IBinTools.h>
#include <sm_argbuffer.h>

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

            SM_GET_IFACE(BINTOOLS, g_pBinTools);

            return true;
        }

        void *FindSig(const char *name)
        {
            void *addr;
            if(!g_pGameConf->GetMemSig(name, &addr))
                throw std::runtime_error("hook : sig not found - " + std::string(name));
            return addr;
        }

        void SDK_OnUnload() {
            gameconfs->CloseGameConfigFile(g_pGameConf);
        }

        // _ZN11CBasePlayer13GiveNamedItemEPKciP13CEconItemViewb
        // CBaseEntity *CBasePlayer::GiveNamedItem(const char *pszName, int iSubType, CEconItemView *, bool removeIfNotCarried)
        CBaseEntity *GivePlayerItem(CBasePlayer *player, const char *item, int iSubType) {
#ifndef _WIN32
            PassInfo pass[4];
            pass[0].flags = PASSFLAG_BYVAL;
            pass[0].type = PassType_Basic;
            pass[0].size = sizeof(const char *);
            pass[1].flags = PASSFLAG_BYVAL;
            pass[1].type = PassType_Basic;
            pass[1].size = sizeof(int);
            pass[2].flags = PASSFLAG_BYVAL;
            pass[2].type = PassType_Basic;
            pass[2].size = sizeof(CEconItemView *);
            pass[3].flags = PASSFLAG_BYVAL;
            pass[3].type = PassType_Basic;
            pass[3].size = sizeof(bool);
            static ICallWrapper *pWrapper = g_pBinTools->CreateCall(FindSig("GiveNamedItem"), CallConv_ThisCall, NULL, pass, 4);

            ArgBuffer<CBasePlayer *, const char *, int , CEconItemView *, bool > vstk(player, item, iSubType, nullptr, true);
            CBaseEntity *ret = nullptr;
            pWrapper->Execute(vstk, &ret);
            return ret;
#else
            static void *addr = FindSig("GiveNamedItem");
			return (*reinterpret_cast<CBaseEntity *(__thiscall *)(const char *, int, CEconItemView *, bool)>(addr))(player, item, iSubType, nullptr, true);
#endif
        }
    }
}