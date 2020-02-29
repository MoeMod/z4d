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
    zombie::OnClientInit(id);
}

void gameplay::Event_OnPlayerSpawn(IGameEvent *pEvent) {
    int id = pEvent->GetInt("userid");
    // send by event, must be connected...
    if (!playerhelpers->GetGamePlayer(id)->IsConnected())
        return;

}

void gameplay::Event_OnRoundStart(IGameEvent *pEvent)
{

    for(int id = 1; id <= playerhelpers->GetMaxClients(); ++id)
    {
        if(!playerhelpers->GetGamePlayer(id)->IsConnected())
            continue;
        gameplay::zmarket::ShowBuyMenu(id);
    }

}
