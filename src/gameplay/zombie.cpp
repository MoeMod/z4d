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

        std::bitset<SM_MAXPLAYERS + 1> g_bitsIsZombie;
        std::bitset<SM_MAXPLAYERS + 1> g_bitsIsOrigin;
        std::array<int, SM_MAXPLAYERS + 1> g_iMaxHealth;
        std::array<int, SM_MAXPLAYERS + 1> g_iMaxArmor;

        Vector g_vecSavedViewPunchAngle;
        Vector g_vecSavedAimPunchAngle;

        bool IsPlayerZombie(int id)
        {
            return g_bitsIsZombie.test(id);
        }

        sm::HookResult<int> OnTakeDamage(CBaseEntity* entity, sm::hack::TakeDamageInfo&)
        {
            if (!sm::IsPlayerAlive(entity))
                return sm::Plugin_Continue;
            auto id = sm::cbase2id(entity);
            if (!IsPlayerZombie(id))
                return sm::Plugin_Continue;
            // TEST 2020/3/1
            g_vecSavedViewPunchAngle = sm::EntProp<Vector>(entity, sm::Prop_Send, "m_viewPunchAngle");
            g_vecSavedAimPunchAngle = sm::EntProp<Vector>(entity, sm::Prop_Send, "m_aimPunchAngle");
            return sm::Plugin_Continue;
        }

        void OnTakeDamagePost(CBaseEntity *entity, sm::hack::TakeDamageInfo &)
        {
            if(!sm::IsPlayerAlive(entity))
                return ;
            auto id = sm::cbase2id(entity);
            if (!IsPlayerZombie(id))
                return ;
            // revert m_Local.m_vecPunchAngle.SetX( flPunch );
            sm::EntProp<Vector>(entity, sm::Prop_Send, "m_viewPunchAngle") = std::exchange(g_vecSavedViewPunchAngle, {});
            sm::EntProp<Vector>(entity, sm::Prop_Send, "m_aimPunchAngle") = std::exchange(g_vecSavedAimPunchAngle, {});
        }

        sm::HookResult<bool> WeaponCanUse(CBaseEntity *player, CBaseCombatWeapon *weapon)
        {
            if (!sm::IsPlayerAlive(player))
                return sm::Plugin_Continue;
            int id = sm::cbase2id(player);
            if (zombie::IsPlayerZombie(id))
            {
                if (const char* classname = sm::GetEdictClassname(sm::cbase2edict(weapon)); !strstr(classname, "knife"))
                {
                    return { sm::Plugin_Handled, false };
                }
            }
            return sm::Plugin_Continue;
        }

        static void ZombieME(int id)
        {
            g_bitsIsZombie.set(id);
            g_iMaxHealth[id] = std::max(g_iMaxHealth[id], 1000);
            CBasePlayer *ent = sm::ent_cast<CBasePlayer *>(id);
            tools::SetHealth(ent, g_iMaxHealth[id], true);
            tools::SetArmor(ent, g_iMaxArmor[id]);
            tools::SetMaxspeedMultiplier(ent, 290.f / 250.f);

            // TODO : crashes
            //tools::SetGravity(ent, 0.8f);

            // Start Weapon
            tools::RemoveAllWeapons(ent);
            auto wpn = sm::sdktools::GivePlayerItem(ent, "weapon_knife");
            sm::sdktools::EquipPlayerWeapon(ent, wpn);

            // Turn Off the FlashLight

            // Player Model
            //SetEntityModel(ent, )

        }

        bool Originate(int id, int iZombieCount, int bIgnoreCheck) {

            if (sm::CallForwardEvent(forwards.OriginatePre, id, iZombieCount) != sm::Plugin_Continue && !bIgnoreCheck)
                return false;

            g_iMaxHealth[id] = ((teammgr::TeamCount(teammgr::ZB_TEAM_HUMAN, true) + teammgr::TeamCount(teammgr::ZB_TEAM_HUMAN, true) / iZombieCount) + 1) * 1000;
            g_iMaxArmor[id] = 1000;
            g_bitsIsOrigin.set(id, true);
            ZombieME(id);

            sm::CallForwardIgnore(forwards.OriginatePost, id, iZombieCount);
            return true;
        }

        bool Infect(int id, int attacker, int bIgnoreCheck) {
            if(bIgnoreCheck)
            {
                if(sm::CallForwardEvent(forwards.InfectPre, id, attacker) != sm::Plugin_Continue)
                    return false;
            }

            if(teammgr::IsAlive(attacker))
            {
                IGamePlayer *igp = sm::IGamePlayerFrom(attacker);
                g_iMaxHealth[id] = std::max<int>(sm::GetClientHealth(igp) * 0.75f, 4000);
                g_iMaxArmor[id] = std::max(sm::GetClientArmor(igp) / 2, 200);
            }
            else
            {
                g_iMaxHealth[id] = 4000;
                g_iMaxArmor[id] = 1000;
            }

            g_bitsIsOrigin.set(id, false);
            ZombieME(id);

            sm::CallForwardIgnore(forwards.InfectPost, id, attacker);
            return true;
        }

        bool Respawn(int id, int bIgnoreCheck)
        {
            g_bitsIsZombie.set(id, false);
            sm::cstrike::CS_UpdateClientModel(sm::ent_cast<CBasePlayer *>(id));
            return true;
        }

        EventListener g_OnTakeDamage_AlivePost_Listener;

        void Init()
        {
        }

        EventListener g_OnTakeDamageListener;
        EventListener g_OnTakeDamagePostListener;
        EventListener g_WeaponCanUseListener;

        void OnClientInit(int id)
        {
            CBaseEntity *player = sm::id2cbase(id);
            g_OnTakeDamageListener = sm::sdkhooks::SDKHookRAII(player, sm::sdkhooks::SDKHook_OnTakeDamage, OnTakeDamage);
            g_OnTakeDamagePostListener = sm::sdkhooks::SDKHookRAII(player, sm::sdkhooks::SDKHook_OnTakeDamagePost, OnTakeDamagePost);
            g_WeaponCanUseListener = sm::sdkhooks::SDKHookRAII(player, sm::sdkhooks::SDKHook_WeaponCanUse, WeaponCanUse);
        }

        void OnClientDisconnected(int id)
        {
            sm::sdkhooks::SDKUnhookRAII(g_OnTakeDamageListener);
            sm::sdkhooks::SDKUnhookRAII(g_OnTakeDamagePostListener);
            sm::sdkhooks::SDKUnhookRAII(g_WeaponCanUseListener);
        }
    }
}