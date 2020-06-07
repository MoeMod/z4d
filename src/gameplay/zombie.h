#pragma once

#include "sm/hook_result.h"
#include "util/EventDispatcher.hpp"

namespace gameplay {
    namespace zombie {
        extern struct {
            EventDispatcher<sm::Action(int id, int iZombieCount)> OriginatePre;
            EventDispatcher<void(int id, int iZombieCount)> OriginatePost;
            EventDispatcher<sm::Action(int id, int attacker)> InfectPre;
            EventDispatcher<void(int id, int attacker)> InfectPost;
            EventDispatcher<sm::Action(int id)> RespawnPre;
            EventDispatcher<void(int id)> RespawnPost;
        } forwards;

        bool IsPlayerZombie(int id);
        bool Originate(int id, int iZombieCount, int bIgnoreCheck);
        bool Infect(int id, int attacker, int bIgnoreCheck);
        bool Respawn(int id, int bIgnoreCheck);

        void Init();
        void OnClientInit(int id);
        void OnClientDisconnected(int id);
    }
}