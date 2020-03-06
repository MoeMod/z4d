//
// Created by 梅峰源 on 2020-02-27.
//

#include "extension.h"

#include <ISmmPlugin.h>
#include <igameevents.h>

#include "event.h"

#include "gameplay/gameplay.h"

namespace event {

    class EventListener : public IGameEventListener2
    {
    public:
        void FireGameEvent(IGameEvent *pEvent) override
        {
            /* Not going to do anything here.
               Just need to add ourselves as a listener to make our hook on IGameEventManager2::FireEvent work */
        }

        int GetEventDebugID() override
        {
            return EVENT_DEBUG_ID_INIT;
        }
    } g_EventListener;

    class EventManager
    {
    public:
        virtual bool OnFireEvent(IGameEvent *pEvent, bool bDontBroadcast)
        {
            if(pEvent && !strcmp(pEvent->GetName(), "round_start"))
            {
                gameplay::Event_OnRoundStart(pEvent);
            }

            if (pEvent && !strcmp(pEvent->GetName(), "player_spawn"))
            {
                gameplay::Event_OnPlayerSpawn(pEvent);
            }

            if (pEvent && !strcmp(pEvent->GetName(), "player_death"))
            {
                gameplay::Event_OnPlayerDeath(pEvent);
            }

            RETURN_META_VALUE(MRES_IGNORED, true);
        }
    } g_EventManager;

    IGameEventManager2 *gameevents = nullptr;

    SH_DECL_HOOK2(IGameEventManager2, FireEvent, SH_NOATTRIB, 0, bool, IGameEvent *, bool);

    void SDK_OnAllLoaded()
    {

    }

    bool SDK_OnMetamodLoad(ISmmAPI *ismm, char *error, size_t maxlen, bool late)
    {
        GET_V_IFACE_CURRENT(GetEngineFactory, gameevents, IGameEventManager2, INTERFACEVERSION_GAMEEVENTSMANAGER2);

        return true;
    }

    bool SDK_OnLoad(char *error, size_t maxlen, bool late)
    {
        gameevents->AddListener(&g_EventListener, "round_start", true);
        SH_ADD_HOOK(IGameEventManager2, FireEvent, gameevents, SH_MEMBER(&g_EventManager, &EventManager::OnFireEvent), false);
        return true;
    }

    void SDK_OnUnload() {
        SH_REMOVE_HOOK(IGameEventManager2, FireEvent, gameevents, SH_MEMBER(&g_EventManager, &EventManager::OnFireEvent), false);
        gameevents->RemoveListener(&g_EventListener);

    }
}