#pragma once


#include "util/EventDispatcher.hpp"

#include "hook_takedamageinfo.h"
#include "cstrike/csgo_const.h"
#include "hook_types.h"

#include "hook_result.h"

#include <typeindex>

namespace sm {
    namespace sdkhooks {

        struct HookTagBase {};
        template<class FuncType> struct HookTag;
        template<class Ret, class...Args> struct HookTag<HookResult<Ret>(Args...)> : HookTagBase
        {
            using DelegateType = EventDispatcher<HookResult<Ret>(Args...)>;
            using ReturnType = Ret;
        };
        template<class...Args> struct HookTag<void(Args...)> : HookTagBase
        {
            using DelegateType = EventDispatcher<void(Args...)>;
            using ReturnType = void;
        };

        constexpr struct : HookTag<HookResult<void>(CBaseEntity *, CBaseEntity *)> {} SDKHook_EndTouch;
        constexpr struct : HookTag<void(CBaseEntity*, const FireBulletsInfo_t &)> {} SDKHook_FireBulletsPost;
        constexpr struct : HookTag<HookResult<int>(CBaseEntity*, TakeDamageInfo &)> {} SDKHook_OnTakeDamage;
        constexpr struct : HookTag<void(CBaseEntity*, TakeDamageInfo &)> {} SDKHook_OnTakeDamagePost;
        constexpr struct : HookTag<HookResult<void>(CBaseEntity*)> {} SDKHook_PreThink;
        constexpr struct : HookTag<HookResult<void>(CBaseEntity*)> {} SDKHook_PostThink;
        constexpr struct : HookTag<HookResult<void>(CBaseEntity*, CCheckTransmitInfo*, bool)> {} SDKHook_SetTransmit;
        constexpr struct : HookTag<HookResult<void>(CBaseEntity*)> {} SDKHook_Spawn;
        constexpr struct : HookTag<HookResult<void>(CBaseEntity*, CBaseEntity*)> {} SDKHook_StartTouch;
        constexpr struct : HookTag<HookResult<void>(CBaseEntity*)> {} SDKHook_Think;
        constexpr struct : HookTag<HookResult<void>(CBaseEntity*, CBaseEntity*)> {} SDKHook_Touch;
        constexpr struct : HookTag<HookResult<void>(CBaseEntity*, TakeDamageInfo&, const Vector&, trace_t*)> {} SDKHook_TraceAttack;
        constexpr struct : HookTag<void(CBaseEntity*, TakeDamageInfo&, const Vector&, trace_t*)> {} SDKHook_TraceAttackPost;
        constexpr struct : HookTag<HookResult<bool>(CBaseEntity*, CBaseCombatWeapon*)> {} SDKHook_WeaponCanSwitchTo;
        constexpr struct : HookTag<HookResult<bool>(CBaseEntity*, CBaseCombatWeapon*)> {} SDKHook_WeaponCanUse;
        constexpr struct : HookTag<HookResult<void>(CBaseEntity*, CBaseCombatWeapon*, const Vector*, const Vector*)> {} SDKHook_WeaponDrop;
        constexpr struct : HookTag<HookResult<void>(CBaseEntity*, CBaseCombatWeapon*)> {} SDKHook_WeaponEquip;
        constexpr struct : HookTag<HookResult<bool>(CBaseEntity*, CBaseCombatWeapon*, int)> {} SDKHook_WeaponSwitch;
        constexpr struct : HookTag<HookResult<bool>(CBaseEntity*, int, int)> {} SDKHook_ShouldCollide;
        constexpr struct : HookTag<void(CBaseEntity*)> {} SDKHook_PreThinkPost;
        constexpr struct : HookTag<void(CBaseEntity*)> {} SDKHook_PostThinkPost;
        constexpr struct : HookTag<void(CBaseEntity*)> {} SDKHook_ThinkPost;
        constexpr struct : HookTag<void(CBaseEntity *, CBaseEntity *)> {} SDKHook_EndTouchPost;
        constexpr struct : HookTag<void(CBaseEntity*, void*)> {} SDKHook_GroundEntChangedPost;
        constexpr struct : HookTag<void(CBaseEntity*)> {} SDKHook_SpawnPost;
        constexpr struct : HookTag<void(CBaseEntity*, CBaseEntity*)> {} SDKHook_StartTouchPost;
        constexpr struct : HookTag<void(CBaseEntity *, CBaseEntity *)> {} SDKHook_TouchPost;
        constexpr struct : HookTag<HookResult<void>(CBaseEntity*, IPhysicsObject*)> {} SDKHook_VPhysicsUpdate;
        constexpr struct : HookTag<void(CBaseEntity*, IPhysicsObject*)> {} SDKHook_VPhysicsUpdatePost;
        constexpr struct : HookTag<void(CBaseEntity*, CBaseCombatWeapon*)> {} SDKHook_WeaponCanSwitchToPost;
        constexpr struct : HookTag<void(CBaseEntity*, CBaseCombatWeapon*)> {} SDKHook_WeaponCanUsePost;
        constexpr struct : HookTag<void(CBaseEntity*, CBaseCombatWeapon*, const Vector*, const Vector*)> {} SDKHook_WeaponDropPost;
        constexpr struct : HookTag<void(CBaseEntity*, CBaseCombatWeapon*)> {} SDKHook_WeaponEquipPost;
        constexpr struct : HookTag<void(CBaseEntity*, CBaseCombatWeapon*, int)> {} SDKHook_WeaponSwitchPost;
        constexpr struct : HookTag<HookResult<void>(CBaseEntity*, CBaseEntity*, CBaseEntity*, USE_TYPE, float)> {} SDKHook_Use;
        constexpr struct : HookTag<void(CBaseEntity*, CBaseEntity*, CBaseEntity*, USE_TYPE, float)> {} SDKHook_UsePost;
        constexpr struct : HookTag<HookResult<bool>(CBaseEntity*)> {} SDKHook_Reload;
        constexpr struct : HookTag<void(CBaseEntity*)> {} SDKHook_ReloadPost;
        constexpr struct : HookTag<HookResult<int>(CBaseEntity*)> {} SDKHook_GetMaxHealth;
        constexpr struct : HookTag<HookResult<void>(CBaseEntity*, CBaseEntity*)> {} SDKHook_Blocked;
        constexpr struct : HookTag<void(CBaseEntity*, CBaseEntity*)> {} SDKHook_BlockedPost;
        constexpr struct : HookTag<HookResult<int>(CBaseEntity*, TakeDamageInfo &)> {} SDKHook_OnTakeDamage_Alive;
        constexpr struct : HookTag<void(CBaseEntity*, TakeDamageInfo &)> {} SDKHook_OnTakeDamage_AlivePost;
        constexpr struct : HookTag<HookResult<bool>(CBaseEntity *)> {} SDKHook_CanBeAutobalanced;

        template<class Tag>
        inline auto GetHookDelegateSingleton() -> typename Tag::DelegateType &
        {
            static typename Tag::DelegateType x;
            return x;
        }

        bool AddGlobalListenerKeeper(EventListener listener);
        bool RemoveGlobalListenerKeeper(EventListener listener);
        void Hook(CBaseEntity *pEnt, const std::type_index &WrappedHookTagType);
        void UnHook(CBaseEntity *pEnt, const std::type_index &WrappedHookTagType);

        // 注意：返回值必须保存到变量，否则事件会直接销毁
        template<class Tag, class Func>
        [[nodiscard]] EventListener SDKHookRAII(CBaseEntity* pEnt, Tag type, Func &&callback)
        {
            auto listener = GetHookDelegateSingleton<Tag>().subscribe(std::forward<Func>(callback));
            Hook(pEnt, typeid(Tag));
            return listener;
        }

        inline void SDKUnhookRAII(EventListener& eventListener)
        {
            eventListener = nullptr;
        }

        bool SDK_OnLoad(char* error, size_t maxlength, bool late);
        void SDK_OnUnload();

        void SDKHooks_TakeDamage(CBaseEntity *pVictim, const TakeDamageInfo &info);
        void SDKHooks_DropWeapon(CBaseEntity *pPlayer, CBaseCombatWeapon *pWeapon, const Vector *pvecTarget, const Vector *pVelocity);

    }

}