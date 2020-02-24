#pragma once

#include <memory>
#include <optional>

#include "EventDispatcher.hpp"

#include "hook_takedamageinfo.h"

class CBaseCombatWeapon;
class IPhysicsObject;

namespace hook {
    using ::META_RES;

    constexpr struct Ignored_t : std::integral_constant<META_RES, MRES_IGNORED> {} Ignored;
    constexpr struct Handled_t : std::integral_constant<META_RES, MRES_HANDLED> {} Handled;
    constexpr struct Override_t : std::integral_constant<META_RES, MRES_OVERRIDE> {} Override;
    constexpr struct Supercede_t : std::integral_constant<META_RES, MRES_SUPERCEDE> {} Supercede;

    template<class T> struct HookResult : HookResult<void> {
        using Base = HookResult<void>;
        constexpr HookResult(Ignored_t v) : Base(v) {}
        constexpr HookResult(Handled_t v) : Base(v) {}
        HookResult(Override_t v, T x) : Base(v), retval(std::move(x)) {}
        HookResult(Supercede_t v, T x) : Base(v), retval(std::move(x)) {}

        HookResult(T x) : HookResult(Supercede, std::move(x)) {}

        ~HookResult() {
            if (Base::value == Override || Base::value == Supercede)
                retval.ret.~T();
        }
        
        union OptionalResult{
            T ret;
            bool ph;

            OptionalResult() : ph(false) {}
            OptionalResult(T x) : ret(std::move(x)) {}
        } retval;
    };
    template<> struct HookResult<void> {
        META_RES value;
        constexpr HookResult(Ignored_t v) : value(v) {}
        constexpr HookResult(Handled_t v) : value(v) {}
        constexpr HookResult(Override_t v) : value(v) {}
        constexpr HookResult(Supercede_t v) : value(v) {}
    };

    struct HookDelegates {
        EventDispatcher<HookResult<bool>()> CanBeAutobalanced;
        EventDispatcher<HookResult<void>(CBaseEntity *)> EndTouch;
        EventDispatcher<HookResult<void>(CBaseEntity *)> EndTouchPost;
        EventDispatcher<HookResult<void>(const FireBulletsInfo_t &)> FireBulletsPost;
        EventDispatcher<HookResult<int>()> GetMaxHealth;
        EventDispatcher<HookResult<void>(void*)> GroundEntChangedPost;
        EventDispatcher<HookResult<int>(TakeDamageInfo &)> OnTakeDamage;
        EventDispatcher<HookResult<int>(TakeDamageInfo &)> OnTakeDamagePost;
        EventDispatcher<HookResult<int>(TakeDamageInfo &)> OnTakeDamage_Alive;
        EventDispatcher<HookResult<int>(TakeDamageInfo &)> OnTakeDamage_AlivePost;
        EventDispatcher<HookResult<void>()> PreThink;
        EventDispatcher<HookResult<void>()> PreThinkPost;
        EventDispatcher<HookResult<void>()> PostThink;
        EventDispatcher<HookResult<void>()> PostThinkPost;
        EventDispatcher<HookResult<bool>()> Reload;
        EventDispatcher<HookResult<bool>()> ReloadPost;
        EventDispatcher<HookResult<void>(CCheckTransmitInfo*, bool)> SetTransmit;
        EventDispatcher<HookResult<bool>(int, int)> ShouldCollide;
        EventDispatcher<HookResult<void>()> Spawn;
        EventDispatcher<HookResult<void>()> SpawnPost;
        EventDispatcher<HookResult<void>(CBaseEntity*)> StartTouch;
        EventDispatcher<HookResult<void>(CBaseEntity*)> StartTouchPost;
        EventDispatcher<HookResult<void>()> Think;
        EventDispatcher<HookResult<void>()> ThinkPost;
        EventDispatcher<HookResult<void>(CBaseEntity*)> Touch;
        EventDispatcher<HookResult<void>(CBaseEntity*)> TouchPost;
        EventDispatcher<HookResult<void>(TakeDamageInfo&, const Vector&, trace_t*)> TraceAttack;
        EventDispatcher<HookResult<void>(TakeDamageInfo&, const Vector&, trace_t*)> TraceAttackPost;
        EventDispatcher<HookResult<void>(CBaseEntity*, CBaseEntity*, USE_TYPE, float)> Use;
        EventDispatcher<HookResult<void>(CBaseEntity*, CBaseEntity*, USE_TYPE, float)> UsePost;
        EventDispatcher<HookResult<void>(IPhysicsObject*)> VPhysicsUpdate;
        EventDispatcher<HookResult<void>(IPhysicsObject*)> VPhysicsUpdatePost;
        EventDispatcher<HookResult<void>(CBaseEntity*)> Blocked;
        EventDispatcher<HookResult<void>(CBaseEntity*)> BlockedPost;
        EventDispatcher<HookResult<bool>(CBaseCombatWeapon*)> WeaponCanSwitchTo;
        EventDispatcher<HookResult<bool>(CBaseCombatWeapon*)> WeaponCanSwitchToPost;
        EventDispatcher<HookResult<bool>(CBaseCombatWeapon*)> WeaponCanUse;
        EventDispatcher<HookResult<bool>(CBaseCombatWeapon*)> WeaponCanUsePost;
        EventDispatcher<HookResult<void>(CBaseCombatWeapon*, const Vector*, const Vector*)> WeaponDrop;
        EventDispatcher<HookResult<void>(CBaseCombatWeapon*, const Vector*, const Vector*)> WeaponDropPost;
        EventDispatcher<HookResult<void>(CBaseCombatWeapon*)> WeaponEquip;
        EventDispatcher<HookResult<void>(CBaseCombatWeapon*)> WeaponEquipPost;
        EventDispatcher<HookResult<bool>(CBaseCombatWeapon*, int)> WeaponSwitch;
        EventDispatcher<HookResult<bool>(CBaseCombatWeapon*, int)> WeaponSwitchPost;
    };


    class CHookList : public HookDelegates {
    private:
        CHookList();
        friend CHookList& hooks();

    public:
        bool SDK_OnLoad(char* error, size_t maxlength, bool late);
        void SetupHooks(CBaseEntity* pEnt);

    public:
        struct list_t;
        static list_t list;
    };
    CHookList & hooks();

    namespace call {
        void Takedamage(CBaseEntity *pEntity, const TakeDamageInfo &);
        void DropWeapon(CBaseEntity *pPlayer, CBaseCombatWeapon *pWeapon, const Vector *pvecTarget, const Vector *pVelocity);
    }


}