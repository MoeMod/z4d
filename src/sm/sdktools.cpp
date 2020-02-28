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

        // _ZN11CBasePlayer13GiveNamedItemEPKciP13CEconItemViewb
        // CBaseEntity *CBasePlayer::GiveNamedItem(const char *pszName, int iSubType, CEconItemView *, bool removeIfNotCarried)
        CBaseEntity *GivePlayerItem(CBasePlayer * player, const char *item, int iSubType) {
        // TESTED 2020/2/28
#if 0
            PassInfo pass[5];
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
            pass[4].flags = PASSFLAG_BYVAL;
            pass[4].type = PassType_Basic;
            pass[4].size = sizeof(CBaseEntity *);

            PassInfo retBuf;
            retBuf.flags = PASSFLAG_BYVAL;
            retBuf.type = PassType_Basic;
            retBuf.size = sizeof(CBasePlayer*);

            static ICallWrapper *pWrapper = g_pBinTools->CreateVCall(FindOffset("GiveNamedItem"), 0, 0, &retBuf, pass, 5);

            ArgBuffer<CBasePlayer *, const char *, int , CEconItemView *, bool, CBaseEntity *> vstk(player, item, iSubType, nullptr, true, nullptr);
            CBaseEntity *ret = nullptr;
            pWrapper->Execute(vstk, &ret);
            return ret;
#else
            static int offset = FindOffset("GiveNamedItem");
            void* addr = (*reinterpret_cast<void***>(player))[offset];
            auto pfn = reinterpret_cast<CBaseEntity * (__fastcall*)(CBasePlayer*, void*, const char*, int, CEconItemView*, bool, CBaseEntity *)>(addr);
			return (*pfn)(player, nullptr, item, iSubType, nullptr, true, nullptr);
#endif
        }
    }
}