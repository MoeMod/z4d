//
// Created by 梅峰源 on 2020-02-27.
//

#include "extension.h"
#include "gameplay.h"

#include "sm/sdkhooks.h"

#include <igameevents.h>

#include "zmarket.h"
#include "teammgr.h"
#include "zombie.h"

bool gameplay::SDK_OnLoad(char *error, size_t maxlength, bool late) {

    zmarket::Init();
    teammgr::Init();
    zombie::Init();

    return true;
}

void gameplay::SDK_OnUnload() {

}

void gameplay::OnClientPostAdminCheck(int id) {
    CBaseEntity *player = gamehelpers->ReferenceToEntity(id);

    sm::sdkhooks::SDKHook(player);
}

void gameplay::Event_OnPlayerSpawnPost(IGameEvent *pEvent) {
    int id = pEvent->GetInt("userid");
    // send by event, must be connected...
    assert(playerhelpers->GetGamePlayer(id)->IsConnected());
    gameplay::zmarket::ShowBuyMenu(id);
}

void gameplay::Event_OnRoundStart(IGameEvent *pEvent)
{

    for(int id = 1; id <= playerhelpers->GetMaxClients(); ++id)
    {
        if(!playerhelpers->GetGamePlayer(id)->IsConnected())
            continue;
        // TODO
    }

}
