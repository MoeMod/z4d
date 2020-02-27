#pragma once


#include "EventDispatcher.hpp"

#include "hook_takedamageinfo.h"
#include "csgo_const.h"
#include "hook_types.h"

#include "hook_result.h"



namespace hook {
    inline namespace sdkhooks {
        struct HookDelegates {
            EventDispatcher<HookResult<bool>(CBaseEntity *)> CanBeAutobalanced;
            EventDispatcher<HookResult<void>(CBaseEntity *, CBaseEntity *)> EndTouch;
            EventDispatcher<HookResult<void>(CBaseEntity *, CBaseEntity *)> EndTouchPost;
            EventDispatcher<HookResult<void>(CBaseEntity*, const FireBulletsInfo_t &)> FireBulletsPost;
            EventDispatcher<HookResult<int>(CBaseEntity*)> GetMaxHealth;
            EventDispatcher<HookResult<void>(CBaseEntity*, void*)> GroundEntChangedPost;
            EventDispatcher<HookResult<int>(CBaseEntity*, TakeDamageInfo &)> OnTakeDamage;
            EventDispatcher<HookResult<int>(CBaseEntity*, TakeDamageInfo &)> OnTakeDamagePost;
            EventDispatcher<HookResult<int>(CBaseEntity*, TakeDamageInfo &)> OnTakeDamage_Alive;
            EventDispatcher<HookResult<int>(CBaseEntity*, TakeDamageInfo &)> OnTakeDamage_AlivePost;
            EventDispatcher<HookResult<void>(CBaseEntity*)> PreThink;
            EventDispatcher<HookResult<void>(CBaseEntity*)> PreThinkPost;
            EventDispatcher<HookResult<void>(CBaseEntity*)> PostThink;
            EventDispatcher<HookResult<void>(CBaseEntity*)> PostThinkPost;
            EventDispatcher<HookResult<bool>(CBaseEntity*)> Reload;
            EventDispatcher<HookResult<bool>(CBaseEntity*)> ReloadPost;
            EventDispatcher<HookResult<void>(CBaseEntity*, CCheckTransmitInfo*, bool)> SetTransmit;
            EventDispatcher<HookResult<bool>(CBaseEntity*, int, int)> ShouldCollide;
            EventDispatcher<HookResult<void>(CBaseEntity*)> Spawn;
            EventDispatcher<HookResult<void>(CBaseEntity*)> SpawnPost;
            EventDispatcher<HookResult<void>(CBaseEntity*, CBaseEntity*)> StartTouch;
            EventDispatcher<HookResult<void>(CBaseEntity*, CBaseEntity*)> StartTouchPost;
            EventDispatcher<HookResult<void>(CBaseEntity*)> Think;
            EventDispatcher<HookResult<void>(CBaseEntity*)> ThinkPost;
            EventDispatcher<HookResult<void>(CBaseEntity*, CBaseEntity*)> Touch;
            EventDispatcher<HookResult<void>(CBaseEntity*, CBaseEntity*)> TouchPost;
            EventDispatcher<HookResult<void>(CBaseEntity*, TakeDamageInfo&, const Vector&, trace_t*)> TraceAttack;
            EventDispatcher<HookResult<void>(CBaseEntity*, TakeDamageInfo&, const Vector&, trace_t*)> TraceAttackPost;
            EventDispatcher<HookResult<void>(CBaseEntity*, CBaseEntity*, CBaseEntity*, USE_TYPE, float)> Use;
            EventDispatcher<HookResult<void>(CBaseEntity*, CBaseEntity*, CBaseEntity*, USE_TYPE, float)> UsePost;
            EventDispatcher<HookResult<void>(CBaseEntity*, IPhysicsObject*)> VPhysicsUpdate;
            EventDispatcher<HookResult<void>(CBaseEntity*, IPhysicsObject*)> VPhysicsUpdatePost;
            EventDispatcher<HookResult<void>(CBaseEntity*, CBaseEntity*)> Blocked;
            EventDispatcher<HookResult<void>(CBaseEntity*, CBaseEntity*)> BlockedPost;
            EventDispatcher<HookResult<bool>(CBaseEntity*, CBaseCombatWeapon*)> WeaponCanSwitchTo;
            EventDispatcher<HookResult<bool>(CBaseEntity*, CBaseCombatWeapon*)> WeaponCanSwitchToPost;
            EventDispatcher<HookResult<bool>(CBaseEntity*, CBaseCombatWeapon*)> WeaponCanUse;
            EventDispatcher<HookResult<bool>(CBaseEntity*, CBaseCombatWeapon*)> WeaponCanUsePost;
            EventDispatcher<HookResult<void>(CBaseEntity*, CBaseCombatWeapon*, const Vector*, const Vector*)> WeaponDrop;
            EventDispatcher<HookResult<void>(CBaseEntity*, CBaseCombatWeapon*, const Vector*, const Vector*)> WeaponDropPost;
            EventDispatcher<HookResult<void>(CBaseEntity*, CBaseCombatWeapon*)> WeaponEquip;
            EventDispatcher<HookResult<void>(CBaseEntity*, CBaseCombatWeapon*)> WeaponEquipPost;
            EventDispatcher<HookResult<bool>(CBaseEntity*, CBaseCombatWeapon*, int)> WeaponSwitch;
            EventDispatcher<HookResult<bool>(CBaseEntity*, CBaseCombatWeapon*, int)> WeaponSwitchPost;
        };


        class CHookList : public HookDelegates {
        private:
            CHookList();
            friend CHookList& hooks();

        public:
            bool SDK_OnLoad(char* error, size_t maxlength, bool late);
            void SDK_OnAllLoaded();
            void SDK_OnUnload();
            void SetupHooks();
            void Hook(CBaseEntity* pEnt);
        };
        CHookList & hooks();

        void SDKHooks_TakeDamage(CBaseEntity *pVictim, const TakeDamageInfo &info);
        void SDKHooks_DropWeapon(CBaseEntity *pPlayer, CBaseCombatWeapon *pWeapon, const Vector *pvecTarget, const Vector *pVelocity);

    }

}