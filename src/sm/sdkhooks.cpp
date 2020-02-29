
#include "extension.h"
#include "sdkhooks.h"

#include <vtable_hook_helper.h>

#include <functional>
#include <string>
#include <map>
#include <set>

class IPhysicsObject;

namespace sm {
    namespace sdkhooks {

        // vtable, tag_type, hookid
        static std::map<std::pair<void *, std::type_index>, int> g_Hooked;

        struct HookList_impl {

            // 注意这里是右值引用
            template<class Ret>
            static constexpr Ret GetReturnValueFrom(HookResult<Ret> &&ret) {
                return std::move(ret.retval.ret);
            }

            static constexpr void GetReturnValueFrom(HookResult<void> &&ret) {
                return void();
            }

            template<class Ret, class...Args, class...ArgsOriginal>
            static Ret ReturnMetaValue(EventDispatcher<HookResult<Ret>(Args...)> &dispatcher, CBaseEntity *pEntity, ArgsOriginal &&...args_original) {
                static_assert(sizeof...(ArgsOriginal) + 1 == sizeof...(Args), "wrong argument");
                HookResult<Ret> ret = dispatcher.dispatch_find_if(pEntity, std::forward<ArgsOriginal>(args_original)..., ShouldBlock, HookResult<Ret>(Ignored));
                RETURN_META_VALUE(ret.value, GetReturnValueFrom(std::move(ret)));
            }

            template<class Ret = void, class...Args, class...ArgsOriginal>
            static Ret ReturnMetaValue(EventDispatcher<void(Args...)> &dispatcher, CBaseEntity *pEntity, ArgsOriginal &&...args_original) {
                static_assert(sizeof...(ArgsOriginal) + 1 == sizeof...(Args), "wrong argument");
                dispatcher.dispatch(pEntity, std::forward<ArgsOriginal>(args_original)...);
                RETURN_META_VALUE(MRES_IGNORED, META_RESULT_ORIG_RET(Ret));
            }

            template<class Ret, class TagType, class...ArgsOriginal>
            static Ret HookImpl(TagType, ArgsOriginal &&...args_original)
            {
                CBaseEntity *pEntity = META_IFACEPTR(CBaseEntity);
                if(g_Hooked.find(std::pair<void *, std::type_index>(CVTableHook(pEntity).GetVTablePtr() , typeid(TagType))) == g_Hooked.end())
                    RETURN_META_VALUE(MRES_IGNORED, META_RESULT_ORIG_RET(Ret));

                return ReturnMetaValue<Ret>(GetHookDelegateSingleton<TagType>(), pEntity, std::forward<ArgsOriginal>(args_original)...);
            }

            /**
             * IServerGameDLL & IVEngineServer Hook Handlers
             */

            const char *Hook_GetMapEntitiesString();

            bool Hook_LevelInit(char const *pMapName, char const *pMapEntities, char const *pOldLevel, char const *pLandmarkName, bool loadGame, bool background);

            /**
             * CBaseEntity Hook Handlers
             */
            bool Hook_CanBeAutobalanced() {
                return HookImpl<decltype(SDKHook_CanBeAutobalanced)::ReturnType>(SDKHook_CanBeAutobalanced);
            }

            void Hook_EndTouch(CBaseEntity *pOther) {
                return HookImpl<decltype(SDKHook_EndTouch)::ReturnType>(SDKHook_EndTouch, pOther);
            }

            void Hook_EndTouchPost(CBaseEntity *pOther) {
                return HookImpl<decltype(SDKHook_EndTouchPost)::ReturnType>(SDKHook_EndTouchPost, pOther);
            }

            void Hook_FireBulletsPost(const FireBulletsInfo_t &info) {
                return HookImpl<decltype(SDKHook_FireBulletsPost)::ReturnType>(SDKHook_FireBulletsPost, info);
            }

            int Hook_GetMaxHealth() {
                return HookImpl<decltype(SDKHook_GetMaxHealth)::ReturnType>(SDKHook_GetMaxHealth);
            }

            void Hook_GroundEntChangedPost(void *pVar) {
                return HookImpl<decltype(SDKHook_GroundEntChangedPost)::ReturnType>(SDKHook_GroundEntChangedPost, pVar);
            }

            int Hook_OnTakeDamage(TakeDamageInfo &info) {
                return HookImpl<decltype(SDKHook_OnTakeDamage)::ReturnType>(SDKHook_OnTakeDamage, info);
            }

            int Hook_OnTakeDamagePost(TakeDamageInfo &info) {
                return HookImpl<int>(SDKHook_OnTakeDamagePost, info);
            }

            int Hook_OnTakeDamage_Alive(TakeDamageInfo &info) {
                return HookImpl<decltype(SDKHook_OnTakeDamage_Alive)::ReturnType>(SDKHook_OnTakeDamage_Alive, info);
            }

            int Hook_OnTakeDamage_AlivePost(TakeDamageInfo &info) {
                return HookImpl<int>(SDKHook_OnTakeDamage_AlivePost, info);
            }

            void Hook_PreThink() {
                return HookImpl<decltype(SDKHook_PreThink)::ReturnType>(SDKHook_PreThink);
            }

            void Hook_PreThinkPost() {
                return HookImpl<decltype(SDKHook_PreThinkPost)::ReturnType>(SDKHook_PreThinkPost);
            }

            void Hook_PostThink() {
                return HookImpl<decltype(SDKHook_PostThink)::ReturnType>(SDKHook_PostThink);
            }

            void Hook_PostThinkPost() {
                return HookImpl<decltype(SDKHook_PostThinkPost)::ReturnType>(SDKHook_PostThinkPost);
            }

            bool Hook_Reload() {
                return HookImpl<decltype(SDKHook_Reload)::ReturnType>(SDKHook_Reload);
            }

            bool Hook_ReloadPost() {
                return HookImpl<bool>(SDKHook_ReloadPost);
            }

            void Hook_SetTransmit(CCheckTransmitInfo *pInfo, bool bAlways) {
                return HookImpl<decltype(SDKHook_SetTransmit)::ReturnType>(SDKHook_SetTransmit, pInfo, bAlways);
            }

            bool Hook_ShouldCollide(int collisonGroup, int contentsMask) {
                return HookImpl<decltype(SDKHook_ShouldCollide)::ReturnType>(SDKHook_ShouldCollide, collisonGroup, contentsMask);
            }

            void Hook_Spawn() {
                return HookImpl<decltype(SDKHook_Spawn)::ReturnType>(SDKHook_Spawn);
            }

            void Hook_SpawnPost() {
                return HookImpl<decltype(SDKHook_SpawnPost)::ReturnType>(SDKHook_SpawnPost);
            }

            void Hook_StartTouch(CBaseEntity *pOther) {
                return HookImpl<decltype(SDKHook_StartTouch)::ReturnType>(SDKHook_StartTouch, pOther);
            }

            void Hook_StartTouchPost(CBaseEntity *pOther) {
                return HookImpl<decltype(SDKHook_StartTouchPost)::ReturnType>(SDKHook_StartTouchPost, pOther);
            }

            void Hook_Think() {
                return HookImpl<decltype(SDKHook_Think)::ReturnType>(SDKHook_Think);
            }

            void Hook_ThinkPost() {
                return HookImpl<decltype(SDKHook_ThinkPost)::ReturnType>(SDKHook_ThinkPost);
            }

            void Hook_Touch(CBaseEntity *pOther) {
                return HookImpl<decltype(SDKHook_Touch)::ReturnType>(SDKHook_Touch, pOther);
            }

            void Hook_TouchPost(CBaseEntity *pOther) {
                return HookImpl<decltype(SDKHook_TouchPost)::ReturnType>(SDKHook_TouchPost, pOther);
            }

            void Hook_TraceAttack(TakeDamageInfo &info, const Vector &vecDir, trace_t *ptr) {
                return HookImpl<decltype(SDKHook_TraceAttack)::ReturnType>(SDKHook_TraceAttack, info, vecDir, ptr);
            }

            void Hook_TraceAttackPost(TakeDamageInfo &info, const Vector &vecDir, trace_t *ptr) {
                return HookImpl<decltype(SDKHook_TraceAttackPost)::ReturnType>(SDKHook_TraceAttackPost, info, vecDir, ptr);
            }

            void Hook_Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value) {
                return HookImpl<decltype(SDKHook_Use)::ReturnType>(SDKHook_Use, pActivator, pCaller, useType, value);
            }

            void Hook_UsePost(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value) {
                return HookImpl<decltype(SDKHook_UsePost)::ReturnType>(SDKHook_UsePost, pActivator, pCaller, useType, value);
            }

            void Hook_VPhysicsUpdate(IPhysicsObject *pPhysics) {
                return HookImpl<decltype(SDKHook_VPhysicsUpdate)::ReturnType>(SDKHook_VPhysicsUpdate, pPhysics);
            }

            void Hook_VPhysicsUpdatePost(IPhysicsObject *pPhysics) {
                return HookImpl<decltype(SDKHook_VPhysicsUpdatePost)::ReturnType>(SDKHook_VPhysicsUpdatePost, pPhysics);
            }

            void Hook_Blocked(CBaseEntity *pOther) {
                return HookImpl<decltype(SDKHook_Blocked)::ReturnType>(SDKHook_Blocked, pOther);
            }

            void Hook_BlockedPost(CBaseEntity *pOther) {
                return HookImpl<decltype(SDKHook_BlockedPost)::ReturnType>(SDKHook_BlockedPost, pOther);
            }

            bool Hook_WeaponCanSwitchTo(CBaseCombatWeapon *pWeapon) {
                return HookImpl<decltype(SDKHook_WeaponCanSwitchTo)::ReturnType>(SDKHook_WeaponCanSwitchTo, pWeapon);
            }

            bool Hook_WeaponCanSwitchToPost(CBaseCombatWeapon *pWeapon) {
                return HookImpl<bool>(SDKHook_WeaponCanSwitchToPost, pWeapon);
            }

            bool Hook_WeaponCanUse(CBaseCombatWeapon *pWeapon) {
                return HookImpl<decltype(SDKHook_WeaponCanUse)::ReturnType>(SDKHook_WeaponCanUse, pWeapon);
            }

            bool Hook_WeaponCanUsePost(CBaseCombatWeapon *pWeapon) {
                return HookImpl<bool>(SDKHook_WeaponCanUsePost, pWeapon);
            }

            void Hook_WeaponDrop(CBaseCombatWeapon *pWeapon, const Vector *pvecTarget, const Vector *pVelocity) {
                return HookImpl<decltype(SDKHook_WeaponDrop)::ReturnType>(SDKHook_WeaponDrop, pWeapon, pvecTarget, pVelocity);
            }

            void Hook_WeaponDropPost(CBaseCombatWeapon *pWeapon, const Vector *pvecTarget, const Vector *pVelocity) {
                return HookImpl<decltype(SDKHook_WeaponDropPost)::ReturnType>(SDKHook_WeaponDropPost, pWeapon, pvecTarget, pVelocity);
            }

            void Hook_WeaponEquip(CBaseCombatWeapon *pWeapon) {
                return HookImpl<decltype(SDKHook_WeaponEquip)::ReturnType>(SDKHook_WeaponEquip, pWeapon);
            }

            void Hook_WeaponEquipPost(CBaseCombatWeapon *pWeapon) {
                return HookImpl<decltype(SDKHook_WeaponEquipPost)::ReturnType>(SDKHook_WeaponEquipPost, pWeapon);
            }

            bool Hook_WeaponSwitch(CBaseCombatWeapon *pWeapon, int viewmodelindex) {
                return HookImpl<decltype(SDKHook_WeaponSwitch)::ReturnType>(SDKHook_WeaponSwitch, pWeapon, viewmodelindex);
            }

            bool Hook_WeaponSwitchPost(CBaseCombatWeapon *pWeapon, int viewmodelindex) {
                return HookImpl<bool>(SDKHook_WeaponSwitchPost, pWeapon, viewmodelindex);
            }

        } s_HookList_impl;

        IGameConfig *g_pGameConf = nullptr;

        SH_DECL_MANUALHOOK1_void(EndTouch, 0, 0, 0, CBaseEntity *);

        SH_DECL_MANUALHOOK1_void(FireBullets, 0, 0, 0, FireBulletsInfo_t const&);

        SH_DECL_MANUALHOOK0(GetMaxHealth, 0, 0, 0, int);

        SH_DECL_MANUALHOOK1_void(GroundEntChanged, 0, 0, 0, void*);

        SH_DECL_MANUALHOOK1(OnTakeDamage, 0, 0, 0, int, TakeDamageInfo &);

        SH_DECL_MANUALHOOK1(OnTakeDamage_Alive, 0, 0, 0, int, TakeDamageInfo &);

        SH_DECL_MANUALHOOK0_void(PreThink, 0, 0, 0);

        SH_DECL_MANUALHOOK0_void(PostThink, 0, 0, 0);

        SH_DECL_MANUALHOOK0(Reload, 0, 0, 0, bool);

        SH_DECL_MANUALHOOK2_void(SetTransmit, 0, 0, 0, CCheckTransmitInfo*, bool);

        SH_DECL_MANUALHOOK2(ShouldCollide, 0, 0, 0, bool, int, int);

        SH_DECL_MANUALHOOK0_void(Spawn, 0, 0, 0);

        SH_DECL_MANUALHOOK1_void(StartTouch, 0, 0, 0, CBaseEntity*);

        SH_DECL_MANUALHOOK0_void(Think, 0, 0, 0);

        SH_DECL_MANUALHOOK1_void(Touch, 0, 0, 0, CBaseEntity*);

        SH_DECL_MANUALHOOK3_void(TraceAttack, 0, 0, 0, TakeDamageInfo &, const Vector&, CGameTrace*);

        SH_DECL_MANUALHOOK4_void(Use, 0, 0, 0, CBaseEntity*, CBaseEntity*, USE_TYPE, float);

        SH_DECL_MANUALHOOK1_void(VPhysicsUpdate, 0, 0, 0, IPhysicsObject*);

        SH_DECL_MANUALHOOK1(Weapon_CanSwitchTo, 0, 0, 0, bool, CBaseCombatWeapon*);

        SH_DECL_MANUALHOOK1(Weapon_CanUse, 0, 0, 0, bool, CBaseCombatWeapon*);

        SH_DECL_MANUALHOOK3_void(Weapon_Drop, 0, 0, 0, CBaseCombatWeapon*, const Vector*, const Vector*);

        SH_DECL_MANUALHOOK1_void(Weapon_Equip, 0, 0, 0, CBaseCombatWeapon*);

        SH_DECL_MANUALHOOK2(Weapon_Switch, 0, 0, 0, bool, CBaseCombatWeapon*, int);

        SH_DECL_MANUALHOOK1_void(Blocked, 0, 0, 0, CBaseEntity*);

        SH_DECL_MANUALHOOK0(CanBeAutobalanced, 0, 0, 0, bool);

#define SET_PRE_true(gamedataname)
#define SET_PRE_false(gamedataname)
#define SET_POST_true(gamedataname)
#define SET_POST_false(gamedataname)

#define CHECKOFFSET(gamedataname, supportsPre, supportsPost) \
    offset = 0; \
    g_pGameConf->GetOffset(#gamedataname, &offset); \
    if (offset > 0) \
    { \
        SH_MANUALHOOK_RECONFIGURE(gamedataname, offset, 0, 0); \
        SET_PRE_##supportsPre(gamedataname) \
        SET_POST_##supportsPost(gamedataname) \
    }

#define CHECKOFFSET_W(gamedataname, supportsPre, supportsPost) \
    offset = 0; \
    g_pGameConf->GetOffset("Weapon_"#gamedataname, &offset); \
    if (offset > 0) \
    { \
        SH_MANUALHOOK_RECONFIGURE(Weapon_##gamedataname, offset, 0, 0); \
        SET_PRE_##supportsPre(Weapon##gamedataname) \
        SET_POST_##supportsPost(Weapon##gamedataname) \
    }

        void SetupHooks() {
            int offset;

            //			gamedata          pre    post
            // (pre is not necessarily a prehook, just named without "Post" appeneded)

            CHECKOFFSET(EndTouch, true, true);
            CHECKOFFSET(FireBullets, false, true);
            CHECKOFFSET(GroundEntChanged, false, true);
            CHECKOFFSET(OnTakeDamage, true, true);
            CHECKOFFSET(OnTakeDamage_Alive, true, true);
            CHECKOFFSET(PreThink, true, true);
            CHECKOFFSET(PostThink, true, true);
            CHECKOFFSET(Reload, true, true);
            CHECKOFFSET(SetTransmit, true, false);
            CHECKOFFSET(ShouldCollide, true, false);
            CHECKOFFSET(Spawn, true, true);
            CHECKOFFSET(StartTouch, true, true);
            CHECKOFFSET(Think, true, true);
            CHECKOFFSET(Touch, true, true);
            CHECKOFFSET(TraceAttack, true, true);
            CHECKOFFSET(Use, true, true);
            CHECKOFFSET_W(CanSwitchTo, true, true);
            CHECKOFFSET_W(CanUse, true, true);
            CHECKOFFSET_W(Drop, true, true);
            CHECKOFFSET_W(Equip, true, true);
            CHECKOFFSET_W(Switch, true, true);
            CHECKOFFSET(VPhysicsUpdate, true, true);
            CHECKOFFSET(Blocked, true, true);
            CHECKOFFSET(CanBeAutobalanced, true, false);

        }

        bool SDK_OnLoad(char *error, size_t maxlength, bool late) {
            char buffer[256];

            buffer[0] = '\0';
            if (!gameconfs->LoadGameConfigFile("sdkhooks.games", &g_pGameConf, buffer, sizeof(buffer))) {
                if (buffer[0]) {
                    g_pSM->Format(error, maxlength, "Could not read sdkhooks.games gamedata: %s", buffer);
                }

                return false;
            }

            SetupHooks();

            return true;
        }

        void Hook(CBaseEntity *pEnt, const std::type_index &WrappedHookTagType) {
            int hookid = 0;
            std::pair<void *, std::type_index> key(CVTableHook(pEnt).GetVTablePtr() , WrappedHookTagType );
            if(g_Hooked.find(key) != g_Hooked.end())
                return;
            else if (WrappedHookTagType ==typeid(SDKHook_EndTouch))
                hookid = SH_ADD_MANUALVPHOOK(EndTouch, pEnt, SH_MEMBER(&s_HookList_impl, &HookList_impl::Hook_EndTouch), false);
            else if (WrappedHookTagType ==typeid(SDKHook_EndTouchPost))
                hookid = SH_ADD_MANUALVPHOOK(EndTouch, pEnt, SH_MEMBER(&s_HookList_impl, &HookList_impl::Hook_EndTouchPost), true);
            else if (WrappedHookTagType ==typeid(SDKHook_FireBulletsPost))
                hookid = SH_ADD_MANUALVPHOOK(FireBullets, pEnt, SH_MEMBER(&s_HookList_impl, &HookList_impl::Hook_FireBulletsPost), true);
            else if (WrappedHookTagType ==typeid(SDKHook_GetMaxHealth))
                hookid = SH_ADD_MANUALVPHOOK(GetMaxHealth, pEnt, SH_MEMBER(&s_HookList_impl, &HookList_impl::Hook_GetMaxHealth), false);
            else if (WrappedHookTagType ==typeid(SDKHook_GroundEntChangedPost))
                hookid = SH_ADD_MANUALVPHOOK(GroundEntChanged, pEnt, SH_MEMBER(&s_HookList_impl, &HookList_impl::Hook_GroundEntChangedPost), true);
            else if (WrappedHookTagType ==typeid(SDKHook_OnTakeDamage))
                hookid = SH_ADD_MANUALVPHOOK(OnTakeDamage, pEnt, SH_MEMBER(&s_HookList_impl, &HookList_impl::Hook_OnTakeDamage), false);
            else if (WrappedHookTagType ==typeid(SDKHook_OnTakeDamagePost))
                hookid = SH_ADD_MANUALVPHOOK(OnTakeDamage, pEnt, SH_MEMBER(&s_HookList_impl, &HookList_impl::Hook_OnTakeDamagePost), true);
            else if (WrappedHookTagType ==typeid(SDKHook_OnTakeDamage_Alive))
                hookid = SH_ADD_MANUALVPHOOK(OnTakeDamage_Alive, pEnt, SH_MEMBER(&s_HookList_impl, &HookList_impl::Hook_OnTakeDamage_Alive), false);
            else if (WrappedHookTagType ==typeid(SDKHook_OnTakeDamage_AlivePost))
                hookid = SH_ADD_MANUALVPHOOK(OnTakeDamage_Alive, pEnt, SH_MEMBER(&s_HookList_impl, &HookList_impl::Hook_OnTakeDamage_AlivePost), true);
            else if (WrappedHookTagType ==typeid(SDKHook_PreThink))
                hookid = SH_ADD_MANUALVPHOOK(PreThink, pEnt, SH_MEMBER(&s_HookList_impl, &HookList_impl::Hook_PreThink), false);
            else if (WrappedHookTagType ==typeid(SDKHook_PreThinkPost))
                hookid = SH_ADD_MANUALVPHOOK(PreThink, pEnt, SH_MEMBER(&s_HookList_impl, &HookList_impl::Hook_PreThinkPost), true);
            else if (WrappedHookTagType ==typeid(SDKHook_PostThink))
                hookid = SH_ADD_MANUALVPHOOK(PostThink, pEnt, SH_MEMBER(&s_HookList_impl, &HookList_impl::Hook_PostThink), false);
            else if (WrappedHookTagType ==typeid(SDKHook_PostThinkPost))
                hookid = SH_ADD_MANUALVPHOOK(PostThink, pEnt, SH_MEMBER(&s_HookList_impl, &HookList_impl::Hook_PostThinkPost), true);
            else if (WrappedHookTagType ==typeid(SDKHook_Reload))
                hookid = SH_ADD_MANUALVPHOOK(Reload, pEnt, SH_MEMBER(&s_HookList_impl, &HookList_impl::Hook_Reload), false);
            else if (WrappedHookTagType ==typeid(SDKHook_ReloadPost))
                hookid = SH_ADD_MANUALVPHOOK(Reload, pEnt, SH_MEMBER(&s_HookList_impl, &HookList_impl::Hook_ReloadPost), true);
            else if (WrappedHookTagType ==typeid(SDKHook_SetTransmit))
                hookid = SH_ADD_MANUALVPHOOK(SetTransmit, pEnt, SH_MEMBER(&s_HookList_impl, &HookList_impl::Hook_SetTransmit), false);
            else if (WrappedHookTagType ==typeid(SDKHook_Spawn))
                hookid = SH_ADD_MANUALVPHOOK(Spawn, pEnt, SH_MEMBER(&s_HookList_impl, &HookList_impl::Hook_Spawn), false);
            else if (WrappedHookTagType ==typeid(SDKHook_SpawnPost))
                hookid = SH_ADD_MANUALVPHOOK(Spawn, pEnt, SH_MEMBER(&s_HookList_impl, &HookList_impl::Hook_SpawnPost), true);
            else if (WrappedHookTagType ==typeid(SDKHook_StartTouch))
                hookid = SH_ADD_MANUALVPHOOK(StartTouch, pEnt, SH_MEMBER(&s_HookList_impl, &HookList_impl::Hook_StartTouch), false);
            else if (WrappedHookTagType ==typeid(SDKHook_StartTouchPost))
                hookid = SH_ADD_MANUALVPHOOK(StartTouch, pEnt, SH_MEMBER(&s_HookList_impl, &HookList_impl::Hook_StartTouchPost), true);
            else if (WrappedHookTagType ==typeid(SDKHook_Think))
                hookid = SH_ADD_MANUALVPHOOK(Think, pEnt, SH_MEMBER(&s_HookList_impl, &HookList_impl::Hook_Think), false);
            else if (WrappedHookTagType ==typeid(SDKHook_ThinkPost))
                hookid = SH_ADD_MANUALVPHOOK(Think, pEnt, SH_MEMBER(&s_HookList_impl, &HookList_impl::Hook_ThinkPost), true);
            else if (WrappedHookTagType ==typeid(SDKHook_Touch))
                hookid = SH_ADD_MANUALVPHOOK(Touch, pEnt, SH_MEMBER(&s_HookList_impl, &HookList_impl::Hook_Touch), false);
            else if (WrappedHookTagType ==typeid(SDKHook_TouchPost))
                hookid = SH_ADD_MANUALVPHOOK(Touch, pEnt, SH_MEMBER(&s_HookList_impl, &HookList_impl::Hook_TouchPost), true);
            else if (WrappedHookTagType ==typeid(SDKHook_TraceAttack))
                hookid = SH_ADD_MANUALVPHOOK(TraceAttack, pEnt, SH_MEMBER(&s_HookList_impl, &HookList_impl::Hook_TraceAttack), false);
            else if (WrappedHookTagType ==typeid(SDKHook_TraceAttackPost))
                hookid = SH_ADD_MANUALVPHOOK(TraceAttack, pEnt, SH_MEMBER(&s_HookList_impl, &HookList_impl::Hook_TraceAttackPost), true);
            else if (WrappedHookTagType ==typeid(SDKHook_Use))
                hookid = SH_ADD_MANUALVPHOOK(Use, pEnt, SH_MEMBER(&s_HookList_impl, &HookList_impl::Hook_Use), false);
            else if (WrappedHookTagType ==typeid(SDKHook_UsePost))
                hookid = SH_ADD_MANUALVPHOOK(Use, pEnt, SH_MEMBER(&s_HookList_impl, &HookList_impl::Hook_UsePost), true);
            else if (WrappedHookTagType ==typeid(SDKHook_VPhysicsUpdate))
                hookid = SH_ADD_MANUALVPHOOK(VPhysicsUpdate, pEnt, SH_MEMBER(&s_HookList_impl, &HookList_impl::Hook_VPhysicsUpdate), false);
            else if (WrappedHookTagType ==typeid(SDKHook_VPhysicsUpdatePost))
                hookid = SH_ADD_MANUALVPHOOK(VPhysicsUpdate, pEnt, SH_MEMBER(&s_HookList_impl, &HookList_impl::Hook_VPhysicsUpdatePost), true);
            else if (WrappedHookTagType ==typeid(SDKHook_WeaponCanSwitchTo))
                hookid = SH_ADD_MANUALVPHOOK(Weapon_CanSwitchTo, pEnt, SH_MEMBER(&s_HookList_impl, &HookList_impl::Hook_WeaponCanSwitchTo), false);
            else if (WrappedHookTagType ==typeid(SDKHook_WeaponCanSwitchToPost))
                hookid = SH_ADD_MANUALVPHOOK(Weapon_CanSwitchTo, pEnt, SH_MEMBER(&s_HookList_impl, &HookList_impl::Hook_WeaponCanSwitchToPost), true);
            else if (WrappedHookTagType ==typeid(SDKHook_WeaponCanUse))
                hookid = SH_ADD_MANUALVPHOOK(Weapon_CanUse, pEnt, SH_MEMBER(&s_HookList_impl, &HookList_impl::Hook_WeaponCanUse), false);
            else if (WrappedHookTagType ==typeid(SDKHook_WeaponCanUsePost))
                hookid = SH_ADD_MANUALVPHOOK(Weapon_CanUse, pEnt, SH_MEMBER(&s_HookList_impl, &HookList_impl::Hook_WeaponCanUsePost), true);
            else if (WrappedHookTagType ==typeid(SDKHook_WeaponDrop))
                hookid = SH_ADD_MANUALVPHOOK(Weapon_Drop, pEnt, SH_MEMBER(&s_HookList_impl, &HookList_impl::Hook_WeaponDrop), false);
            else if (WrappedHookTagType ==typeid(SDKHook_WeaponDropPost))
                hookid = SH_ADD_MANUALVPHOOK(Weapon_Drop, pEnt, SH_MEMBER(&s_HookList_impl, &HookList_impl::Hook_WeaponDropPost), true);
            else if (WrappedHookTagType ==typeid(SDKHook_WeaponEquip))
                hookid = SH_ADD_MANUALVPHOOK(Weapon_Equip, pEnt, SH_MEMBER(&s_HookList_impl, &HookList_impl::Hook_WeaponEquip), false);
            else if (WrappedHookTagType ==typeid(SDKHook_WeaponEquipPost))
                hookid = SH_ADD_MANUALVPHOOK(Weapon_Equip, pEnt, SH_MEMBER(&s_HookList_impl, &HookList_impl::Hook_WeaponEquipPost), true);
            else if (WrappedHookTagType ==typeid(SDKHook_WeaponSwitch))
                hookid = SH_ADD_MANUALVPHOOK(Weapon_Switch, pEnt, SH_MEMBER(&s_HookList_impl, &HookList_impl::Hook_WeaponSwitch), false);
            else if (WrappedHookTagType ==typeid(SDKHook_WeaponSwitchPost))
                hookid = SH_ADD_MANUALVPHOOK(Weapon_Switch, pEnt, SH_MEMBER(&s_HookList_impl, &HookList_impl::Hook_WeaponSwitchPost), true);
            else if (WrappedHookTagType ==typeid(SDKHook_ShouldCollide))
                hookid = SH_ADD_MANUALVPHOOK(ShouldCollide, pEnt, SH_MEMBER(&s_HookList_impl, &HookList_impl::Hook_ShouldCollide), false);
            else if (WrappedHookTagType ==typeid(SDKHook_Blocked))
                hookid = SH_ADD_MANUALVPHOOK(Blocked, pEnt, SH_MEMBER(&s_HookList_impl, &HookList_impl::Hook_Blocked), false);
            else if (WrappedHookTagType ==typeid(SDKHook_BlockedPost))
                hookid = SH_ADD_MANUALVPHOOK(Blocked, pEnt, SH_MEMBER(&s_HookList_impl, &HookList_impl::Hook_BlockedPost), true);
            else if (WrappedHookTagType ==typeid(SDKHook_CanBeAutobalanced))
                hookid = SH_ADD_MANUALVPHOOK(CanBeAutobalanced, pEnt, SH_MEMBER(&s_HookList_impl, &HookList_impl::Hook_CanBeAutobalanced), false);

            g_Hooked.emplace(key, hookid);
        }

        void SDK_OnUnload() {
            gameconfs->CloseGameConfigFile(g_pGameConf);
        }

        void SDKHooks_DropWeapon(CBaseEntity *pPlayer, CBaseCombatWeapon *pWeapon, const Vector *pvecTarget, const Vector *pVelocity) {
            SH_MCALL(pPlayer, Weapon_Drop)(pWeapon, pvecTarget, pVelocity);
        }

        void SDKHooks_TakeDamage(CBaseEntity *pVictim, TakeDamageInfo &info) {
            SH_MCALL(pVictim, OnTakeDamage)(info);
        }
    }

}