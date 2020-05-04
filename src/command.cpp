#include "extension.h"
#include "command.h"

#include "command.h"

#include "gameplay/gameplay.h"

namespace command {

    SH_DECL_HOOK1_void(ConCommand, Dispatch, SH_NOATTRIB, false, const CCommand&);
    SH_DECL_HOOK1_void(IServerGameClients, SetCommandClient, SH_NOATTRIB, false, int);

    ICvar* icvar = nullptr;
    IServerGameClients* serverClients = nullptr;

    bool SDK_OnMetamodLoad(ISmmAPI* ismm, char* error, size_t maxlen, bool late)
    {
        GET_V_IFACE_CURRENT(GetServerFactory, serverClients, IServerGameClients, INTERFACEVERSION_SERVERGAMECLIENTS);
        GET_V_IFACE_CURRENT(GetEngineFactory, icvar, ICvar, CVAR_INTERFACE_VERSION);
        return true;
    }

    class SayHook
    {
    public:
        void DispatchSay(const CCommand& command)
        {
            bool handled = false;
            handled |= gameplay::OnClientSay(last_command_client_, command, false);
            if(handled)
                RETURN_META(MRES_SUPERCEDE);
            RETURN_META(MRES_IGNORED);
        }
        void DispatchSayTeam(const CCommand& command)
        {
            bool handled = false;
            handled |= gameplay::OnClientSay(last_command_client_, command, true);
            if(handled)
                RETURN_META(MRES_SUPERCEDE);
            RETURN_META(MRES_IGNORED);
        }
        void SetCommandClient(int client)
        {
            last_command_client_ = client + 1;
        }
        int last_command_client_ = 0;
    } g_SayHook;

    static int g_hookDispatchSay;
    static int g_hookDispatchSayTeam;
    static int g_hookSetCommandClient;

    bool SDK_OnLoad(char* error, size_t maxlength, bool late)
    {
        ConCommand *say = icvar->FindCommand("say");
        ConCommand *say_team = icvar->FindCommand("say_team");

        g_hookDispatchSay = SH_ADD_HOOK(ConCommand, Dispatch, say, SH_MEMBER(&g_SayHook, &SayHook::DispatchSay), false);
        g_hookDispatchSayTeam = SH_ADD_HOOK(ConCommand, Dispatch, say_team, SH_MEMBER(&g_SayHook, &SayHook::DispatchSayTeam), false);
        g_hookSetCommandClient = SH_ADD_HOOK(IServerGameClients, SetCommandClient, serverClients, SH_MEMBER(&g_SayHook, &SayHook::SetCommandClient), false);
        return true;
    }

    void SDK_OnUnload()
    {
        SH_REMOVE_HOOK_ID(g_hookDispatchSay);
        SH_REMOVE_HOOK_ID(g_hookDispatchSayTeam);
        SH_REMOVE_HOOK_ID(g_hookSetCommandClient);
    }

}