#include "extension.h"
#include "sourcemod.h"

#include "sm/sdkhooks.h"
#include "sm/sdktools.h"
#include "sm/cstrike.h"

namespace sm {
    inline namespace sourcemod {
        IGameConfig *g_pGameConf = nullptr;
        ICvar *icvar = nullptr;
        IServerPluginHelpers* serverpluginhelpers = NULL;

        inline namespace hudtext {
            int g_HudMsgNum = -1;
        }

        bool SDK_OnLoad(char *error, size_t maxlen, bool late) {
            char conf_error[255];
            if (!gameconfs->LoadGameConfigFile("core.games", &g_pGameConf, conf_error, sizeof(conf_error)))
            {
                if (error)
                {
                    ke::SafeSprintf(error, maxlen, "Could not read core.games: %s", conf_error);
                }
                return false;
            }
            sm::sdkhooks::SDK_OnLoad(error, maxlen, late);
            sm::sdktools::SDK_OnLoad(error, maxlen, late);
            sm::cstrike::SDK_OnLoad(error, maxlen, late);

            const char *key = g_pGameConf->GetKeyValue("HudTextMsg");
            g_HudMsgNum = usermsgs->GetMessageIndex(key);

            return true;
        }

        void SDK_OnUnload() {
            sm::sdkhooks::SDK_OnUnload();
            sm::sdktools::SDK_OnUnload();
            sm::cstrike::SDK_OnUnload();

        }

        bool SDK_OnMetamodLoad(ISmmAPI *ismm, char *error, size_t maxlen, bool late)
        {
            GET_V_IFACE_CURRENT(GetEngineFactory, icvar, ICvar, CVAR_INTERFACE_VERSION);
            GET_V_IFACE_CURRENT(GetEngineFactory, serverpluginhelpers, IServerPluginHelpers, INTERFACEVERSION_ISERVERPLUGINHELPERS);

            return true;
        }
    }
}

