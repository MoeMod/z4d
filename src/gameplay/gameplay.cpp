//
// Created by 梅峰源 on 2020-02-27.
//

#include "extension.h"
#include "sm/sourcemod.h"
#include "sm/sdkhooks.h"
#include "gameplay.h"

#include <igameevents.h>

#include "alarm.h"
#include "zmarket.h"
#include "teammgr.h"
#include "zombie.h"
#include "iplocation.h"
#include "random_reciter.h"
#include "votekick.h"
#include "mapmgr.h"
#include "rtv.h"
#include "say_menu.h"
#include "qqlogin.h"
#include "itemown.h"
#include "item_grenadepack.h"
#include "chat.h"
#include "mod/mod.h"

#include <string>

namespace gameplay {

    static const sp_nativeinfo_t natives[] =
    {
        { "x_item_consume",			itemown::x_item_consume },
        { "x_item_consume_async",			itemown::x_item_consume_async },
        { "x_item_give",			itemown::x_item_give },
        { "x_item_give_async",			itemown::x_item_give_async },
        { "x_item_get",			itemown::x_item_get },
        { "x_alarm_push",			alarm::x_alarm_push },
        { "x_alarm_insert",			alarm::x_alarm_insert },
        { "x_alarm_timertip",			alarm::x_alarm_timertip },
        { "x_alarm_kill",			alarm::x_alarm_kill },
        { nullptr, 		nullptr }
    };

    bool SDK_OnLoad(char *error, size_t maxlength, bool late) {

        zmarket::Init();
        teammgr::Init();
        zombie::Init();
        iplocation::Init();
        random_reciter::Init();
        votekick::Init();
        mapmgr::Init();
        rtv::Init();
        qqlogin::Init();
        say_menu::Init();

        g_pShareSys->AddNatives(myself, natives);

        mod::Init();
        return true;
    }

    void SDK_OnUnload() {

    }

    void OnClientPreAdminCheck(int id) {
        qqlogin::OnClientPreAdminCheck(id);
    }

    void OnClientPostAdminCheck(int id) {
        zombie::OnClientInit(id);
        itemown::OnClientInit(id);
        itemown::grenadepack::OnClientInit(id);
    }

    void OnClientPutInServer(int id) {
        iplocation::OnClientPutInServer(id);
        mod::g_pModRunning->OnClientPutInServer(id);
    }

    void OnClientDisconnected(int id) {
        zombie::OnClientDisconnected(id);
        rtv::OnClientDisconnected(id);
    }

    void Event_OnPlayerSpawn(IGameEvent *pEvent) {
        mod::g_pModRunning->Event_OnPlayerSpawn(pEvent);

        int id = pEvent->GetInt("userid");
        // send by event, must be connected...
        if (!playerhelpers->GetGamePlayer(id)->IsConnected())
            return;

    }

    void Event_OnPlayerDeath(IGameEvent *pEvent) {
        mod::g_pModRunning->Event_OnPlayerDeath(pEvent);

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

    void Event_OnRoundStart(IGameEvent *pEvent)
    {
        alarm::Event_NewRound();
        random_reciter::Event_NewRound();
        itemown::grenadepack::Event_NewRound();

        mod::g_pModRunning->Event_OnRoundStart(pEvent);
    }

    void Event_OnRoundEnd(IGameEvent* pEvent)
    {
        mod::g_pModRunning->Event_OnRoundEnd(pEvent);
    }

    void Event_OnWinPanelRound(IGameEvent* pEvent)
    {
        mod::g_pModRunning->Event_OnWinPanelRound(pEvent);
    }

    void Event_OnPlayerTeam(IGameEvent* pEvent)
    {
        mod::g_pModRunning->Event_OnPlayerTeam(pEvent);
    }

    bool OnClientCommand(edict_t *pEntity, const CCommand & command)
    {
        if (!strcmp(command.Arg(0), "rebuy"))
            return say_menu::ShowMainMenu(sm::edict2id(pEntity)), true;

        return mod::g_pModRunning->OnClientCommand(pEntity, command);
    }

    bool OnClientSay(int id, const CCommand& command, bool team)
    {
        // 不知道为什么id=0也会跑到这里
        if (id == 0)
            return false;

        random_reciter::OnClientSay(id, command, team);

        if(!strcmp(command.Arg(1), "rtv") || !strcmp(command.Arg(1), "\"rtv\""))
            return rtv::OnSayRTV(id), true;

        if(!strcmp(command.Arg(1), "menu") || !strcmp(command.Arg(1), "\"menu\"") || !strcmp(command.Arg(1), "\"!menu\"") || !strcmp(command.Arg(1), "\"/menu\""))
            return say_menu::ShowMainMenu(id), true;

        return chat::OnClientSay(id, command.Arg(1), team);
    }

    void OnServerLoad()
    {
        mod::g_pModRunning->OnServerLoad();
    }

    void OnMapStart()
    {
        mod::g_pModRunning->OnMapStart();
    }
}