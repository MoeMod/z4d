
#include "extension.h"
#include "hook_mp.h"

#include <extensions/IBinTools.h>
#include <extensions/ISDKHooks.h>
#include <extensions/ISDKTools.h>
#include <sm_argbuffer.h>

#include <functional>
#include <string>

class IPhysicsObject;

namespace hook {

	struct HookList_impl {
		static constexpr bool ShouldContinue_value(META_RES value) { return value != MRES_IGNORED; }
		static constexpr bool ShouldContinue(const HookResult<void>& hr) { return ShouldContinue_value(hr.value); }

		// 注意这里是右值引用
		template<class Ret>
		static constexpr Ret GetReturnValueFrom(HookResult<Ret> &&ret)
		{
			return std::move(ret.retval.ret);
		}
		static constexpr void GetReturnValueFrom(HookResult<void>&& ret)
		{
			return void();
		}

		template<class Ret, class...Args, class...ArgsOriginal> 
		Ret ReturnMetaValue(EventDispatcher<HookResult<Ret>(Args...)>& dispatcher, ArgsOriginal &&...args_original)
		{
			static_assert(sizeof...(ArgsOriginal) + 1 == sizeof...(Args), "wrong argument");
			CBaseEntity* pEntity = META_IFACEPTR(CBaseEntity);
			HookResult<Ret> ret = dispatcher.dispatch_find_if(pEntity, std::forward<ArgsOriginal>(args_original)..., ShouldContinue, HookResult<Ret>(Ignored));
			RETURN_META_VALUE(ret.value, GetReturnValueFrom(std::move(ret)));
		}

		/**
		 * IServerGameDLL & IVEngineServer Hook Handlers
		 */

		const char* Hook_GetMapEntitiesString();
		bool Hook_LevelInit(char const* pMapName, char const* pMapEntities, char const* pOldLevel, char const* pLandmarkName, bool loadGame, bool background);

		/**
		 * CBaseEntity Hook Handlers
		 */
		bool Hook_CanBeAutobalanced()
		{
			return ReturnMetaValue(hooks().CanBeAutobalanced);
		}
		void Hook_EndTouch(CBaseEntity* pOther)
		{
			return ReturnMetaValue(hooks().EndTouch, pOther);
		}
		void Hook_EndTouchPost(CBaseEntity* pOther)
		{
			return ReturnMetaValue(hooks().EndTouchPost, pOther);
		}
		void Hook_FireBulletsPost(const FireBulletsInfo_t& info) 
		{
			return ReturnMetaValue(hooks().FireBulletsPost, info);
		}
		int Hook_GetMaxHealth()
		{
			return ReturnMetaValue(hooks().GetMaxHealth);
		}
		void Hook_GroundEntChangedPost(void* pVar) 
		{
			return ReturnMetaValue(hooks().GroundEntChangedPost, pVar);
		}
		int Hook_OnTakeDamage(TakeDamageInfo& info)
		{
			return ReturnMetaValue(hooks().OnTakeDamage, info);
		}
		int Hook_OnTakeDamagePost(TakeDamageInfo& info)
		{
			return ReturnMetaValue(hooks().OnTakeDamagePost, info);
		}
		int Hook_OnTakeDamage_Alive(TakeDamageInfo& info)
		{
			return ReturnMetaValue(hooks().OnTakeDamage_Alive, info);
		}
		int Hook_OnTakeDamage_AlivePost(TakeDamageInfo& info)
		{
			return ReturnMetaValue(hooks().OnTakeDamage_AlivePost, info);
		}
		void Hook_PreThink()
		{
			return ReturnMetaValue(hooks().PreThink);
		}
		void Hook_PreThinkPost()
		{
			return ReturnMetaValue(hooks().PreThinkPost);
		}
		void Hook_PostThink()
		{
			return ReturnMetaValue(hooks().PostThink);
		}
		void Hook_PostThinkPost()
		{
			return ReturnMetaValue(hooks().PostThinkPost);
		}
		bool Hook_Reload()
		{
			return ReturnMetaValue(hooks().Reload);
		}
		bool Hook_ReloadPost()
		{
			return ReturnMetaValue(hooks().ReloadPost);
		}
		void Hook_SetTransmit(CCheckTransmitInfo* pInfo, bool bAlways)
		{
			return ReturnMetaValue(hooks().SetTransmit, pInfo, bAlways);
		}
		bool Hook_ShouldCollide(int collisonGroup, int contentsMask)
		{
			return ReturnMetaValue(hooks().ShouldCollide, collisonGroup, contentsMask);
		}
		void Hook_Spawn()
		{
			return ReturnMetaValue(hooks().Spawn);
		}
		void Hook_SpawnPost()
		{
			return ReturnMetaValue(hooks().SpawnPost);
		}
		void Hook_StartTouch(CBaseEntity* pOther)
		{
			return ReturnMetaValue(hooks().StartTouch, pOther);
		}
		void Hook_StartTouchPost(CBaseEntity* pOther)
		{
			return ReturnMetaValue(hooks().StartTouchPost, pOther);
		}
		void Hook_Think()
		{
			return ReturnMetaValue(hooks().Think);
		}
		void Hook_ThinkPost()
		{
			return ReturnMetaValue(hooks().ThinkPost);
		}
		void Hook_Touch(CBaseEntity* pOther)
		{
			return ReturnMetaValue(hooks().Touch, pOther);
		}
		void Hook_TouchPost(CBaseEntity* pOther)
		{
			return ReturnMetaValue(hooks().TouchPost, pOther);
		}
		void Hook_TraceAttack(TakeDamageInfo& info, const Vector& vecDir, trace_t* ptr)
		{
			return ReturnMetaValue(hooks().TraceAttack, info, vecDir, ptr);
		}
		void Hook_TraceAttackPost(TakeDamageInfo& info, const Vector& vecDir, trace_t* ptr)
		{
			return ReturnMetaValue(hooks().TraceAttackPost, info, vecDir, ptr);
		}
		void Hook_Use(CBaseEntity* pActivator, CBaseEntity* pCaller, USE_TYPE useType, float value)
		{
			return ReturnMetaValue(hooks().Use, pActivator, pCaller, useType, value);
		}
		void Hook_UsePost(CBaseEntity* pActivator, CBaseEntity* pCaller, USE_TYPE useType, float value)
		{
			return ReturnMetaValue(hooks().UsePost, pActivator, pCaller, useType, value);
		}
		void Hook_VPhysicsUpdate(IPhysicsObject* pPhysics)
		{
			return ReturnMetaValue(hooks().VPhysicsUpdate, pPhysics);
		}
		void Hook_VPhysicsUpdatePost(IPhysicsObject* pPhysics)
		{
			return ReturnMetaValue(hooks().VPhysicsUpdatePost, pPhysics);
		}
		void Hook_Blocked(CBaseEntity* pOther)
		{
			return ReturnMetaValue(hooks().Blocked, pOther);
		}
		void Hook_BlockedPost(CBaseEntity* pOther)
		{
			return ReturnMetaValue(hooks().BlockedPost, pOther);
		}
		bool Hook_WeaponCanSwitchTo(CBaseCombatWeapon* pWeapon)
		{
			return ReturnMetaValue(hooks().WeaponCanSwitchTo, pWeapon);
		}
		bool Hook_WeaponCanSwitchToPost(CBaseCombatWeapon* pWeapon)
		{
			return ReturnMetaValue(hooks().WeaponCanSwitchToPost, pWeapon);
		}
		bool Hook_WeaponCanUse(CBaseCombatWeapon* pWeapon)
		{
			return ReturnMetaValue(hooks().WeaponCanUse, pWeapon);
		}
		bool Hook_WeaponCanUsePost(CBaseCombatWeapon* pWeapon)
		{
			return ReturnMetaValue(hooks().WeaponCanUsePost, pWeapon);
		}
		void Hook_WeaponDrop(CBaseCombatWeapon* pWeapon, const Vector* pvecTarget, const Vector* pVelocity)
		{
			return ReturnMetaValue(hooks().WeaponDrop, pWeapon, pvecTarget, pVelocity);
		}
		void Hook_WeaponDropPost(CBaseCombatWeapon* pWeapon, const Vector* pvecTarget, const Vector* pVelocity)
		{
			return ReturnMetaValue(hooks().WeaponDropPost, pWeapon, pvecTarget, pVelocity);
		}
		void Hook_WeaponEquip(CBaseCombatWeapon* pWeapon)
		{
			return ReturnMetaValue(hooks().WeaponEquip, pWeapon);
		}
		void Hook_WeaponEquipPost(CBaseCombatWeapon* pWeapon)
		{
			return ReturnMetaValue(hooks().WeaponEquipPost, pWeapon);
		}
		bool Hook_WeaponSwitch(CBaseCombatWeapon* pWeapon, int viewmodelindex)
		{
			return ReturnMetaValue(hooks().WeaponSwitch, pWeapon, viewmodelindex);
		}
		bool Hook_WeaponSwitchPost(CBaseCombatWeapon* pWeapon, int viewmodelindex)
		{
			return ReturnMetaValue(hooks().WeaponSwitchPost, pWeapon, viewmodelindex);
		}

	} s_HookList_impl;

	IGameConfig* g_pGameConf = nullptr;
	IBinTools *g_pBinTools = nullptr;
	ISDKHooks *g_pSDKHooks = nullptr;
	ISDKTools *g_pSDKTools = nullptr;

	SH_DECL_MANUALHOOK1_void(EndTouch, 0, 0, 0, CBaseEntity *);
	SH_DECL_MANUALHOOK1_void(FireBullets, 0, 0, 0, FireBulletsInfo_t const&);
	SH_DECL_MANUALHOOK0(GetMaxHealth, 0, 0, 0, int);
	SH_DECL_MANUALHOOK1_void(GroundEntChanged, 0, 0, 0, void*);
	SH_DECL_MANUALHOOK1(OnTakeDamage, 0, 0, 0, int, TakeDamageInfo&);
	SH_DECL_MANUALHOOK1(OnTakeDamage_Alive, 0, 0, 0, int, TakeDamageInfo&);
	SH_DECL_MANUALHOOK0_void(PreThink, 0, 0, 0);
	SH_DECL_MANUALHOOK0_void(PostThink, 0, 0, 0);
	SH_DECL_MANUALHOOK0(Reload, 0, 0, 0, bool);
	SH_DECL_MANUALHOOK2_void(SetTransmit, 0, 0, 0, CCheckTransmitInfo*, bool);
	SH_DECL_MANUALHOOK2(ShouldCollide, 0, 0, 0, bool, int, int);
	SH_DECL_MANUALHOOK0_void(Spawn, 0, 0, 0);
	SH_DECL_MANUALHOOK1_void(StartTouch, 0, 0, 0, CBaseEntity*);
	SH_DECL_MANUALHOOK0_void(Think, 0, 0, 0);
	SH_DECL_MANUALHOOK1_void(Touch, 0, 0, 0, CBaseEntity*);
	SH_DECL_MANUALHOOK3_void(TraceAttack, 0, 0, 0, TakeDamageInfo&, const Vector&, CGameTrace*);
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

	bool CHookList::SDK_OnLoad(char* error, size_t maxlength, bool late)
	{
		char buffer[256];
		g_pSM->BuildPath(Path_SM, buffer, sizeof(buffer) - 1, "/extensions/sdkhooks.ext." PLATFORM_LIB_EXT);
		if (libsys->PathExists(buffer) && libsys->IsPathFile(buffer))
		{
			g_pSM->Format(error, maxlength - 1, "SDKHooks 2.x cannot load while old version (sdkhooks.ext." PLATFORM_LIB_EXT ") is still in extensions dir");
			return false;
		}

		g_pSM->BuildPath(Path_SM, buffer, sizeof(buffer) - 1, "/gamedata/sdkhooks.games.txt");
		if (libsys->PathExists(buffer) && libsys->IsPathFile(buffer))
		{
			g_pSM->Format(error, maxlength - 1, "SDKHooks 2.x cannot load while old gamedata file (sdkhooks.games.txt) is still in gamedata dir");
			return false;
		}

		buffer[0] = '\0';
		if (!gameconfs->LoadGameConfigFile("z4d.games", &g_pGameConf, buffer, sizeof(buffer)))
		{
			if (buffer[0])
			{
				g_pSM->Format(error, maxlength, "Could not read sdkhooks.games gamedata: %s", buffer);
			}

			return false;
		}

		return true;
	}

	void CHookList::SDK_OnAllLoaded() {

		SM_GET_LATE_IFACE(BINTOOLS, g_pBinTools);

		SM_GET_LATE_IFACE(SDKHOOKS, g_pSDKHooks);
		SM_GET_LATE_IFACE(SDKTOOLS, g_pSDKTools);

		return void();
	}

	void CHookList::Hook(CBaseEntity* pEnt)
	{
		SH_ADD_MANUALVPHOOK(EndTouch, pEnt, SH_MEMBER(&s_HookList_impl, &HookList_impl::Hook_EndTouch), false);
		SH_ADD_MANUALVPHOOK(EndTouch, pEnt, SH_MEMBER(&s_HookList_impl, &HookList_impl::Hook_EndTouchPost), true);
		SH_ADD_MANUALVPHOOK(FireBullets, pEnt, SH_MEMBER(&s_HookList_impl, &HookList_impl::Hook_FireBulletsPost), true);
		SH_ADD_MANUALVPHOOK(GetMaxHealth, pEnt, SH_MEMBER(&s_HookList_impl, &HookList_impl::Hook_GetMaxHealth), false);
		SH_ADD_MANUALVPHOOK(GroundEntChanged, pEnt, SH_MEMBER(&s_HookList_impl, &HookList_impl::Hook_GroundEntChangedPost), true);
		SH_ADD_MANUALVPHOOK(OnTakeDamage, pEnt, SH_MEMBER(&s_HookList_impl, &HookList_impl::Hook_OnTakeDamage), false);
		SH_ADD_MANUALVPHOOK(OnTakeDamage, pEnt, SH_MEMBER(&s_HookList_impl, &HookList_impl::Hook_OnTakeDamagePost), true);
		SH_ADD_MANUALVPHOOK(OnTakeDamage_Alive, pEnt, SH_MEMBER(&s_HookList_impl, &HookList_impl::Hook_OnTakeDamage_Alive), false);
		SH_ADD_MANUALVPHOOK(OnTakeDamage_Alive, pEnt, SH_MEMBER(&s_HookList_impl, &HookList_impl::Hook_OnTakeDamage_AlivePost), true);
		SH_ADD_MANUALVPHOOK(PreThink, pEnt, SH_MEMBER(&s_HookList_impl, &HookList_impl::Hook_PreThink), false);
		SH_ADD_MANUALVPHOOK(PreThink, pEnt, SH_MEMBER(&s_HookList_impl, &HookList_impl::Hook_PreThinkPost), true);
		SH_ADD_MANUALVPHOOK(PostThink, pEnt, SH_MEMBER(&s_HookList_impl, &HookList_impl::Hook_PostThink), false);
		SH_ADD_MANUALVPHOOK(PostThink, pEnt, SH_MEMBER(&s_HookList_impl, &HookList_impl::Hook_PostThinkPost), true);
		SH_ADD_MANUALVPHOOK(Reload, pEnt, SH_MEMBER(&s_HookList_impl, &HookList_impl::Hook_Reload), false);
		SH_ADD_MANUALVPHOOK(Reload, pEnt, SH_MEMBER(&s_HookList_impl, &HookList_impl::Hook_ReloadPost), true);
		SH_ADD_MANUALVPHOOK(SetTransmit, pEnt, SH_MEMBER(&s_HookList_impl, &HookList_impl::Hook_SetTransmit), false);
		SH_ADD_MANUALVPHOOK(Spawn, pEnt, SH_MEMBER(&s_HookList_impl, &HookList_impl::Hook_Spawn), false);
		SH_ADD_MANUALVPHOOK(Spawn, pEnt, SH_MEMBER(&s_HookList_impl, &HookList_impl::Hook_SpawnPost), true);
		SH_ADD_MANUALVPHOOK(StartTouch, pEnt, SH_MEMBER(&s_HookList_impl, &HookList_impl::Hook_StartTouch), false);
		SH_ADD_MANUALVPHOOK(StartTouch, pEnt, SH_MEMBER(&s_HookList_impl, &HookList_impl::Hook_StartTouchPost), true);
		SH_ADD_MANUALVPHOOK(Think, pEnt, SH_MEMBER(&s_HookList_impl, &HookList_impl::Hook_Think), false);
		SH_ADD_MANUALVPHOOK(Think, pEnt, SH_MEMBER(&s_HookList_impl, &HookList_impl::Hook_ThinkPost), true);
		SH_ADD_MANUALVPHOOK(Touch, pEnt, SH_MEMBER(&s_HookList_impl, &HookList_impl::Hook_Touch), false);
		SH_ADD_MANUALVPHOOK(Touch, pEnt, SH_MEMBER(&s_HookList_impl, &HookList_impl::Hook_TouchPost), true);
		SH_ADD_MANUALVPHOOK(TraceAttack, pEnt, SH_MEMBER(&s_HookList_impl, &HookList_impl::Hook_TraceAttack), false);
		SH_ADD_MANUALVPHOOK(TraceAttack, pEnt, SH_MEMBER(&s_HookList_impl, &HookList_impl::Hook_TraceAttackPost), true);
		SH_ADD_MANUALVPHOOK(Use, pEnt, SH_MEMBER(&s_HookList_impl, &HookList_impl::Hook_Use), false);
		SH_ADD_MANUALVPHOOK(Use, pEnt, SH_MEMBER(&s_HookList_impl, &HookList_impl::Hook_UsePost), true);
		SH_ADD_MANUALVPHOOK(VPhysicsUpdate, pEnt, SH_MEMBER(&s_HookList_impl, &HookList_impl::Hook_VPhysicsUpdate), false);
		SH_ADD_MANUALVPHOOK(VPhysicsUpdate, pEnt, SH_MEMBER(&s_HookList_impl, &HookList_impl::Hook_VPhysicsUpdatePost), true);
		SH_ADD_MANUALVPHOOK(Weapon_CanSwitchTo, pEnt, SH_MEMBER(&s_HookList_impl, &HookList_impl::Hook_WeaponCanSwitchTo), false);
		SH_ADD_MANUALVPHOOK(Weapon_CanSwitchTo, pEnt, SH_MEMBER(&s_HookList_impl, &HookList_impl::Hook_WeaponCanSwitchToPost), true);
		SH_ADD_MANUALVPHOOK(Weapon_CanUse, pEnt, SH_MEMBER(&s_HookList_impl, &HookList_impl::Hook_WeaponCanUse), false);
		SH_ADD_MANUALVPHOOK(Weapon_CanUse, pEnt, SH_MEMBER(&s_HookList_impl, &HookList_impl::Hook_WeaponCanUsePost), true);
		SH_ADD_MANUALVPHOOK(Weapon_Drop, pEnt, SH_MEMBER(&s_HookList_impl, &HookList_impl::Hook_WeaponDrop), false);
		SH_ADD_MANUALVPHOOK(Weapon_Drop, pEnt, SH_MEMBER(&s_HookList_impl, &HookList_impl::Hook_WeaponDropPost), true);
		SH_ADD_MANUALVPHOOK(Weapon_Equip, pEnt, SH_MEMBER(&s_HookList_impl, &HookList_impl::Hook_WeaponEquip), false);
		SH_ADD_MANUALVPHOOK(Weapon_Equip, pEnt, SH_MEMBER(&s_HookList_impl, &HookList_impl::Hook_WeaponEquipPost), true);
		SH_ADD_MANUALVPHOOK(Weapon_Switch, pEnt, SH_MEMBER(&s_HookList_impl, &HookList_impl::Hook_WeaponSwitch), false);
		SH_ADD_MANUALVPHOOK(Weapon_Switch, pEnt, SH_MEMBER(&s_HookList_impl, &HookList_impl::Hook_WeaponSwitchPost), true);
		SH_ADD_MANUALVPHOOK(ShouldCollide, pEnt, SH_MEMBER(&s_HookList_impl, &HookList_impl::Hook_ShouldCollide), false);
		SH_ADD_MANUALVPHOOK(Blocked, pEnt, SH_MEMBER(&s_HookList_impl, &HookList_impl::Hook_Blocked), false);
		SH_ADD_MANUALVPHOOK(Blocked, pEnt, SH_MEMBER(&s_HookList_impl, &HookList_impl::Hook_BlockedPost), true);
		SH_ADD_MANUALVPHOOK(CanBeAutobalanced, pEnt, SH_MEMBER(&s_HookList_impl, &HookList_impl::Hook_CanBeAutobalanced), false);
	}

	void CHookList::SetupHooks()
	{
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

	CHookList::CHookList() = default;

	CHookList & hooks()
	{
		static CHookList x;
		return x;
	}

	void CHookList::SDK_OnUnload()
	{
		gameconfs->CloseGameConfigFile(g_pGameConf);
	}



	inline namespace call {

		void *FindSig(const char *name)
		{
			void *addr;
			if(!g_pGameConf->GetMemSig("TerminateRound", &addr))
				throw std::runtime_error("hook : sig not found - " + std::string(name));
			return addr;
		}

		void TerminateRound(CGameRules *gamerules, float delay, CSRoundEndReason_e reason) {

#ifndef _WIN32
			PassInfo pass[4];
			pass[0].flags = PASSFLAG_BYVAL;
			pass[0].type = PassType_Float;
			pass[0].size = sizeof(float);
			pass[1].flags = PASSFLAG_BYVAL;
			pass[1].type = PassType_Basic;
			pass[1].size = sizeof(int);
			pass[2].flags = PASSFLAG_BYVAL;
			pass[2].type = PassType_Basic;
			pass[2].size = sizeof(int);
			pass[3].flags = PASSFLAG_BYVAL;
			pass[3].type = PassType_Basic;
			pass[3].size = sizeof(int);
			static ICallWrapper *pWrapper = g_pBinTools->CreateCall(FindSig("TerminateRound"), CallConv_ThisCall, NULL, pass, 4);

			ArgBuffer<void*, float, int, int, int> vstk(gamerules, delay, reason, 0, 0);
			pWrapper->Execute(vstk, nullptr);
#else
			static void *addr = FindSig("TerminateRound");
			__asm
			{
			push 0
			push 0
			push reason
			movss xmm1, delay
			mov ecx, gamerules
			call addr
			}
#endif
		}

		void CS_TerminateRound(float delay, CSRoundEndReason_e reason) {
			return TerminateRound(static_cast<CGameRules *>(g_pSDKTools->GetGameRules()), delay, reason);
		}

		void CS_RespawnPlayer(CBaseEntity *pEntity) {
			static auto pWrapper = g_pBinTools->CreateCall(FindSig("RoundRespawn"), CallConv_ThisCall, NULL, NULL, 0);
			pWrapper->Execute(&pEntity, NULL);
		}

		void CS_UpdateClientModel(CBaseEntity *pEntity) {
			static auto pWrapper = g_pBinTools->CreateCall(FindSig("SetModelFromClass"), CallConv_ThisCall, NULL, NULL, 0);
			pWrapper->Execute(&pEntity, NULL);
		}

		void SwitchTeam(CGameRules *gamerules, CBaseEntity *pEntity, CSTeam_e team) {

#ifndef _WIN32
			PassInfo pass[1];
			pass[0].flags = PASSFLAG_BYVAL;
			pass[0].type = PassType_Basic;
			pass[0].size = sizeof(int);
			static ICallWrapper *pWrapper = g_pBinTools->CreateCall(FindSig("SwitchTeam"), CallConv_ThisCall, NULL, pass, 4);

			ArgBuffer<CBaseEntity*, int> vstk(pEntity, team);
			pWrapper->Execute(vstk, nullptr);
#else
			static void *addr = FindSig("SwitchTeam");
			__asm
			{
			push team
			mov ecx, pEntity
			mov ebx, gamerules
			call addr
			}
#endif
		}

		void CS_SwitchTeam(CBaseEntity *pEntity, CSTeam_e team) {
			return SwitchTeam(static_cast<CGameRules *>(g_pSDKTools->GetGameRules()), pEntity, team);
		}

		void CS_DropWeapon(CBaseEntity *pEntity, CBaseCombatWeapon *pWeapon, bool toss) {
			static auto pWrapper = g_pBinTools->CreateCall(FindSig("DropWeaponBB"), CallConv_ThisCall, NULL, NULL, 0);
			ArgBuffer<CBaseEntity*, CBaseCombatWeapon*, bool> vstk(pEntity, pWeapon, toss);
			pWrapper->Execute(vstk, NULL);
		}

		void SDKHooks_DropWeapon(CBaseEntity *pPlayer, CBaseCombatWeapon *pWeapon, const Vector *pvecTarget, const Vector *pVelocity) {
			SH_MCALL(pPlayer, Weapon_Drop)(pWeapon, pvecTarget, pVelocity);
		}

		void SDKHooks_TakeDamage(CBaseEntity *pVictim, TakeDamageInfo &info) {
			SH_MCALL(pVictim, OnTakeDamage)(info);
		}

	}

}