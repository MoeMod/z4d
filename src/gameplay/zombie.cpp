//
// Created by 梅峰源 on 2020-02-28.
//

#include <IPlayerHelpers.h>
#include "extension.h"
#include "zombie.h"
#include "teammgr.h"
#include "sm/sourcemod.h"
#include "sm/sdkhooks.h"
#include "sm/sdktools.h"
#include "mathlib.h"

#include "tools.h"

#include <bitset>
#include <array>
#include <algorithm>


namespace gameplay {
    namespace zombie {
        decltype(forwards) forwards;

        std::bitset<SM_MAXPLAYERS + 1> g_bitsIsOrigin;
        std::array<int, SM_MAXPLAYERS + 1> g_iMaxHealth;
        std::array<int, SM_MAXPLAYERS + 1> g_iMaxArmor;

        Vector g_vecSavedViewPunchAngle;
        Vector g_vecSavedAimPunchAngle;

        HookResult<int> OnTakeDamage(CBaseEntity* entity, sm::TakeDamageInfo&)
        {
            // TEST 2020/3/1
            g_vecSavedViewPunchAngle = sm::EntProp<Vector>(entity, sm::Prop_Send, "m_viewPunchAngle");
            g_vecSavedAimPunchAngle = sm::EntProp<Vector>(entity, sm::Prop_Send, "m_aimPunchAngle");
            return sm::Ignored;
        }

        void OnTakeDamagePost(CBaseEntity *entity, sm::TakeDamageInfo &)
        {
            if(!sm::IsPlayerAlive(entity))
                return ;
            // revert m_Local.m_vecPunchAngle.SetX( flPunch );
            sm::EntProp<Vector>(entity, sm::Prop_Send, "m_viewPunchAngle") = std::exchange(g_vecSavedViewPunchAngle, {});
            sm::EntProp<Vector>(entity, sm::Prop_Send, "m_aimPunchAngle") = std::exchange(g_vecSavedAimPunchAngle, {});
        }

        static void ZombieME(int id)
        {
            g_iMaxHealth[id] = std::max(g_iMaxHealth[id], 1000);
            CBaseEntity *ent = sm::id2cbase(id);
            tools::SetHealth(ent, g_iMaxHealth[id], true);
            tools::SetArmor(ent, g_iMaxArmor[id]);
            tools::SetMaxspeedMultiplier(ent, 290.f / 250.f);
            tools::SetGravity(ent, 0.8f);

            // Start Weapon
            tools::RemoveAllWeapons(ent);

            // Turn Off the FlashLight

            // Player Model
            //SetEntityModel(ent, )

        }

        void Originate(int id, int iZombieCount, int bIgnoreCheck) {
            if(bIgnoreCheck)
            {
                if(forwards.OriginatePre.dispatch_find_if(id, iZombieCount, sm::ShouldBlock, sm::HookResult<void>()) != sm::Ignored)
                    return;
            }
            forwards.OriginateAct.dispatch(id, iZombieCount);

            g_iMaxHealth[id] = ((TeamCount(teammgr::Z4D_TEAM_HUMAN, true) + TeamCount(teammgr::Z4D_TEAM_ZOMBIE, true) / iZombieCount) + 1) * 1000;
            g_iMaxArmor[id] = 1000;
            g_bitsIsOrigin.set(id, true);
            teammgr::Team_Set(id, teammgr::Z4D_TEAM_ZOMBIE);
            ZombieME(id);

            forwards.OriginatePost.dispatch(id, iZombieCount);
        }

        void Infect(int id, int attacker, int bIgnoreCheck) {
            if(bIgnoreCheck)
            {
                if(forwards.InfectPre.dispatch_find_if(id, attacker, sm::ShouldBlock, sm::HookResult<void>()) != sm::Ignored)
                    return;
            }
            forwards.InfectAct.dispatch(id, attacker);

            if(teammgr::IsAlive(attacker))
            {
                IGamePlayer *igp = sm::IGamePlayerFrom(attacker);
                g_iMaxHealth[id] = std::max<int>(sm::GetHealth(igp) * 0.75f, 4000);
                g_iMaxArmor[id] = std::max(sm::GetArmorValue(igp) / 2, 200);
            }
            else
            {
                g_iMaxHealth[id] = 4000;
                g_iMaxArmor[id] = 1000;
            }

            g_bitsIsOrigin.set(id, false);
            teammgr::Team_Set(id, teammgr::Z4D_TEAM_ZOMBIE);
            ZombieME(id);

            forwards.InfectPost.dispatch(id, attacker);
        }

        EventListener g_OnTakeDamage_AlivePost_Listener;

        void Init()
        {
        }

        EventListener g_OnTakeDamageListener;
        EventListener g_OnTakeDamagePostListener;

        void OnClientInit(int id)
        {
            CBaseEntity *player = gamehelpers->ReferenceToEntity(id);
            g_OnTakeDamageListener = sm::sdkhooks::SDKHook(player, sm::sdkhooks::SDKHook_OnTakeDamage_Alive, OnTakeDamage);
            g_OnTakeDamagePostListener = sm::sdkhooks::SDKHook(player, sm::sdkhooks::SDKHook_OnTakeDamage_AlivePost, OnTakeDamagePost);
        }
    }
}