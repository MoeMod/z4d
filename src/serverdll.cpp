#include "extension.h"

#include "serverdll.h"

#include "gameplay/gameplay.h"
#include "HyDatabase.h"

namespace serverdll {

    class ServerDLLHooks
    {
    public:
        void Hook_ServerActivate(edict_t *pEdictList, int edictCount, int clientMax)
        {
            HyDatabase().Start();
            gameplay::OnServerLoad();
            gameplay::OnMapStart();
        }

        bool Hook_LevelInit(const char *pMapName,
                            char const *pMapEntities,
                            char const *pOldLevel,
                            char const *pLandmarkName,
                            bool loadGame,
                            bool background)
        {
            return true;
        }

        void Hook_GameFrame(bool simulating)
        {
            /**
             * simulating:
             * ***********
             * true  | game is ticking
             * false | game is not ticking
             */
        }

        void Hook_LevelShutdown(void) {}
        void Hook_GameShutdown(void) {}
        void Hook_DLLShutdown(void) {}

        void Hook_ServerHibernationUpdate(bool bHibernating)
        {
            if(bHibernating)
                HyDatabase().Hibernate();
        }

    } g_ServerDLLHooks;

    SH_DECL_HOOK6(IServerGameDLL, LevelInit, SH_NOATTRIB, 0, bool, char const *, char const *, char const *, char const *, bool, bool);
    SH_DECL_HOOK3_void(IServerGameDLL, ServerActivate, SH_NOATTRIB, 0, edict_t *, int, int);
    SH_DECL_HOOK1_void(IServerGameDLL, GameFrame, SH_NOATTRIB, 0, bool);
    SH_DECL_HOOK0_void(IServerGameDLL, LevelShutdown, SH_NOATTRIB, 0);
    SH_DECL_HOOK0_void(IServerGameDLL, GameShutdown, SH_NOATTRIB, 0);
    SH_DECL_HOOK0_void(IServerGameDLL, DLLShutdown, SH_NOATTRIB, 0);
    SH_DECL_HOOK1_void(IServerGameDLL, ServerHibernationUpdate, SH_NOATTRIB, 0, bool);

    IServerGameDLL *server = nullptr;

    bool SDK_OnMetamodLoad(ISmmAPI *ismm, char *error, size_t maxlen, bool late)
    {
        GET_V_IFACE_ANY(GetServerFactory, server, IServerGameDLL, INTERFACEVERSION_SERVERGAMEDLL);

        return true;
    }

    bool SDK_OnLoad(char *error, size_t maxlen, bool late)
    {
        SH_ADD_HOOK_MEMFUNC(IServerGameDLL, LevelInit, server, &g_ServerDLLHooks, &ServerDLLHooks::Hook_LevelInit, true);
        SH_ADD_HOOK_MEMFUNC(IServerGameDLL, ServerActivate, server, &g_ServerDLLHooks, &ServerDLLHooks::Hook_ServerActivate, true);
        SH_ADD_HOOK_MEMFUNC(IServerGameDLL, GameFrame, server, &g_ServerDLLHooks, &ServerDLLHooks::Hook_GameFrame, true);
        SH_ADD_HOOK_MEMFUNC(IServerGameDLL, LevelShutdown, server, &g_ServerDLLHooks, &ServerDLLHooks::Hook_LevelShutdown, false);
        SH_ADD_HOOK_MEMFUNC(IServerGameDLL, GameShutdown, server, &g_ServerDLLHooks, &ServerDLLHooks::Hook_GameShutdown, false);
        SH_ADD_HOOK_MEMFUNC(IServerGameDLL, DLLShutdown, server, &g_ServerDLLHooks, &ServerDLLHooks::Hook_DLLShutdown, false);
        SH_ADD_HOOK_MEMFUNC(IServerGameDLL, ServerHibernationUpdate, server, &g_ServerDLLHooks, &ServerDLLHooks::Hook_ServerHibernationUpdate, false);
        return true;
    }

    void SDK_OnUnload()
    {
        SH_REMOVE_HOOK_MEMFUNC(IServerGameDLL, LevelInit, server, &g_ServerDLLHooks, &ServerDLLHooks::Hook_LevelInit, true);
        SH_REMOVE_HOOK_MEMFUNC(IServerGameDLL, ServerActivate, server, &g_ServerDLLHooks, &ServerDLLHooks::Hook_ServerActivate, true);
        SH_REMOVE_HOOK_MEMFUNC(IServerGameDLL, GameFrame, server, &g_ServerDLLHooks, &ServerDLLHooks::Hook_GameFrame, true);
        SH_REMOVE_HOOK_MEMFUNC(IServerGameDLL, LevelShutdown, server, &g_ServerDLLHooks, &ServerDLLHooks::Hook_LevelShutdown, false);
        SH_REMOVE_HOOK_MEMFUNC(IServerGameDLL, GameShutdown, server, &g_ServerDLLHooks, &ServerDLLHooks::Hook_GameShutdown, false);
        SH_REMOVE_HOOK_MEMFUNC(IServerGameDLL, ServerHibernationUpdate, server, &g_ServerDLLHooks, &ServerDLLHooks::Hook_ServerHibernationUpdate, false);
    }
}