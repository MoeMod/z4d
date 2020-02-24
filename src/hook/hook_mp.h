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
        void SetupHooks();
        void Hook(CBaseEntity* pEnt);

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