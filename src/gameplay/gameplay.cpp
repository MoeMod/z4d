//
// Created by 梅峰源 on 2020-02-27.
//

#include "extension.h"
#include "gameplay.h"

#include "sm/sdkhooks.h"

#include <igameevents.h>

#include "alarm.h"
#include "zmarket.h"
#include "teammgr.h"
#include "zombie.h"
#include "iplocation.h"

bool gameplay::SDK_OnLoad(char *error, size_t maxlength, bool late) {

    zmarket::Init();
    teammgr::Init();
    zombie::Init();
    iplocation::Init();

    return true;
}

void gameplay::SDK_OnUnload() {

}

void gameplay::OnClientPostAdminCheck(int id) {
    zombie::OnClientInit(id);
    iplocation::OnClientInit(id);
}

void gameplay::Event_OnPlayerSpawn(IGameEvent *pEvent) {
    int id = pEvent->GetInt("userid");
    // send by event, must be connected...
    if (!playerhelpers->GetGamePlayer(id)->IsConnected())
        return;

}

void gameplay::Event_OnPlayerDeath(IGameEvent *pEvent) {
    int victim = playerhelpers->GetClientOfUserId(pEvent->GetInt("userid"));
    int attacker = playerhelpers->GetClientOfUserId(pEvent->GetInt("attacker"));
    bool headshot = pEvent->GetBool("headshot");
    bool penetrated = pEvent->GetBool("penetrated");
    bool revenge = pEvent->GetBool("revenge");
    bool dominated = pEvent->GetBool("dominated");
    int assister = playerhelpers->GetClientOfUserId(pEvent->GetBool("assister"));
    // send by event, must be connected...
    if (!playerhelpers->GetGamePlayer(victim)->IsConnected())
        return;

    alarm::SendKillEvent(attacker, victim, alarm::ALARMTYPE_KILL);
}

void gameplay::Event_OnRoundStart(IGameEvent *pEvent)
{
    alarm::Event_NewRound();

    for(int id = 1; id <= playerhelpers->GetMaxClients(); ++id)
    {
        if(!playerhelpers->GetGamePlayer(id)->IsConnected())
            continue;
        //zmarket::ShowBuyMenu(id);
    }

}
