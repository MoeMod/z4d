#pragma once

#include "sm/hook_result.h"
#include "util/EventDispatcher.hpp"

namespace gameplay {
    namespace zombie {
        using sm::HookResult;
        extern struct {
            EventDispatcher<HookResult<void>(int id, int iZombieCount)> OriginatePre;
            EventDispatcher<void(int id, int iZombieCount)> OriginateAct;
            EventDispatcher<void(int id, int iZombieCount)> OriginatePost;
            EventDispatcher<HookResult<void>(int id, int attacker)> InfectPre;
            EventDispatcher<void(int id, int attacker)> InfectAct;
            EventDispatcher<void(int id, int attacker)> InfectPost;
            EventDispatcher<HookResult<void>(int id)> RespawnPre;
            EventDispatcher<void(int id)> RespawnAct;
            EventDispatcher<void(int id)> RespawnPost;
        } forwards;

        void Originate(int id, int iZombieCount, int bIgnoreCheck);
        void Infect(int id, int attacker, int bIgnoreCheck);
        void Respawn(int id, int bIgnoreCheck);

        void Init();
    }
}