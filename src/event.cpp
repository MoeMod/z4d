//
// Created by 梅峰源 on 2020-02-27.
//

#include "extension.h"

#include <ISmmPlugin.h>
#include <igameevents.h>

#include "event.h"

#include "gameplay/gameplay.h"

namespace event {

    class EventManager : public IGameEventListener2
    {
    public:
        void FireGameEvent(IGameEvent* pEvent) override
        {
            if (pEvent && !strcmp(pEvent->GetName(), "round_start"))
            {
                gameplay::Event_OnRoundStart(pEvent);
            }

            if (pEvent && !strcmp(pEvent->GetName(), "round_end"))
            {
                gameplay::Event_OnRoundEnd(pEvent);
            }

            if (pEvent && !strcmp(pEvent->GetName(), "player_spawn"))
            {
                gameplay::Event_OnPlayerSpawn(pEvent);
            }

            if (pEvent && !strcmp(pEvent->GetName(), "player_death"))
            {
                gameplay::Event_OnPlayerDeath(pEvent);
            }

            if (pEvent && !strcmp(pEvent->GetName(), "player_team"))
            {
                gameplay::Event_OnPlayerTeam(pEvent);
            }
        }

        int GetEventDebugID() override
        {
            return EVENT_DEBUG_ID_INIT;
        }
    } g_EventManager;

    IGameEventManager2 *gameevents = nullptr;

    bool SDK_OnMetamodLoad(ISmmAPI *ismm, char *error, size_t maxlen, bool late)
    {
        GET_V_IFACE_CURRENT(GetEngineFactory, gameevents, IGameEventManager2, INTERFACEVERSION_GAMEEVENTSMANAGER2);

        return true;
    }

    bool SDK_OnLoad(char *error, size_t maxlen, bool late)
    {

        gameevents->AddListener(&g_EventManager, "round_start", true);
        gameevents->AddListener(&g_EventManager, "player_spawn", true);
        gameevents->AddListener(&g_EventManager, "player_death", true);

        return true;
    }

    void SDK_OnUnload() {

        gameevents->RemoveListener(&g_EventManager);

    }
}