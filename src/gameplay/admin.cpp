//
// Created by 梅峰源 on 2020-05-05.
//

#include "extension.h"
#include "admin.h"
#include "sm/sourcemod.h"
#include "sm/sdktools.h"
#include "sm/cstrike.h"
#include "sm/ranges.h"
#include "tools.h"
#include "rtv.h"
#include "mapmgr.h"
#include "command.h"
#include "itemown.h"
#include "gameplay.h"
#include "weapon_list.h"

#include "util/ImMenu.hpp"

#include <functional>
#include <optional>

namespace gameplay {

    namespace admin {

        constexpr const char * g_szReasonForPunishPlayer[] = {
                "根据玩家投票要求",
                "发生BUG需要处理",
                "模式或活动的需要",
                "恶意堵路",
                "基建恶意拆迁/堵门",
                "越狱/匪镇乱杀",
                "越狱/逃跑不听指挥",
                "越狱/逃跑乱用道具",
                "恶意抢狱长/指挥",
                "浪费游戏资源",
                "长时间不在位置(挂机)",
                "恶意坑队友(闪光弹/震荡弹)",
                "恶意卡屏(烟雾弹/手电筒)",
                "使用禁用道具/途径",
                "卡神(PING非常高)",
                "未知来源的玩家(假人)",
                "使用透视外挂",
                "使用自瞄外挂",
                "使用加速/瞬移外挂",
                "使用连跳类辅助脚本",
                "使用刷分挂机脚本",
                "地图BUG干扰游戏(卡天卡门)",
                "利用BUG刷任务/道具",
                "利用BUG炸服/卡服",
                "发送无关广告/拉人",
                "辱骂服务器及管理员",
                "祖安玩家",
                "聊天灌水/麦克风噪音",
                "搞黄色",
                "恶意发起投票换图/踢人",
                "对于管理员有过分要求",
                "白嫖道具/活动",
                "恶意刷分",
                "装萌新",
                "这人傻逼到无法形容",
                "不喜欢小白白",
                "长得太丑",
                "不符合社会主义核心价值观",
                "有男/女朋友"
        };

        constexpr const char * g_szReasonForMap[] = {
                "根据玩家投票要求",
                "发生BUG需要处理",
                "模式或活动的需要",
                "这个地图不适合这个模式",
                "这个地图非常不平衡",
                "这个地图BUG非常多",
                "这个地图刚才玩过了",
                "这个地图大家玩腻了",
                "需要更新服务器模式"
        };

        void KickAndBan(int target, int time = 30)
        {
            auto pPlayer = playerhelpers->GetGamePlayer(target);
            engine->ServerCommand((std::string() + "banid " + std::to_string(time) + " " + pPlayer->GetAuthString() + "\n").c_str());
            engine->ServerCommand((std::string() + "addip " + std::to_string(time) + " " + pPlayer->GetIPAddress() + "\n").c_str());
            engine->ServerCommand("writeip");
            engine->ServerCommand("writeid");
            gamehelpers->AddDelayedKick(target, pPlayer->GetUserId(), "[死神CS社区] Kicked : 您被强制踢出了");
        }

        void LogAndPrintAdmin(int adminid, const std::string &action, const std::string &reason, const std::string &targetname = "")
        {
            auto igpAdmin = sm::IGamePlayerFrom(adminid);
            auto adminname = sm::GetClientName(igpAdmin);
            if(targetname.empty())
                sm::PrintToChatAll((std::string() + " \x05[死神CS社区]\x01 管理员 \x02" + adminname + " \x01因为" + reason + "使用权限 \x02" + action + " \x01").c_str());
            else
                sm::PrintToChatAll((std::string() + " \x05[死神CS社区]\x01 管理员 \x02" + adminname + " \x01因为" + reason + "把 \x02" + targetname + " \x01进行 \x02" + action + " \x01").c_str());
            // TODO : log to mysql
        }

        template<class InputIter, class CallbackFn = void(*)()>
        void ShowActionReasonMenu(int adminid, const std::string& szAction, InputIter reasonsfirst, InputIter reasonslast, CallbackFn&& fnAction, const std::string& szTarget = "")
        {
            static_assert(std::is_invocable<CallbackFn>::value);
            util::ImMenu([adminid, szAction, fnAction, reasonsfirst, reasonslast, szTarget](auto context) {
                context.begin("管理员装逼菜单 / Admin\n"
                    "选择进行" + szAction + "的原因");

                std::for_each(reasonsfirst, reasonslast, [&](auto &&reason){
                    if (context.item(reason, reason))
                    {
                        fnAction();
                        LogAndPrintAdmin(adminid, szAction, reason, szTarget);
                    }
                });

                context.end(adminid);
                });
        }

        template<class CallbackFn = void(*)(int target), class FilterFn = bool(*)(int)>
        void ShowSelectTargetPlayerMenu(int adminid, const std::string& szAction, CallbackFn&& fnAction, FilterFn&& fnFilter = [](int) { return true;  })
        {
            static_assert(std::is_invocable<CallbackFn, int>::value);
            util::ImMenu([adminid, szAction, fnAction, fnFilter](auto &&context){
                context.begin("管理员装逼菜单 / Admin\n"
                              "选择进行" + szAction + "的目标玩家");
                for(int target : sm::ranges::Players() | sm::ranges::Connected())
                {
                    auto igp = sm::ent_cast<IGamePlayer *>(target);
                    if(!igp || !igp->IsConnected())
                        continue;

                    std::string targetname = igp->GetName();
                    if (!fnFilter(target))
                        context.disabled();
                    if(context.item(igp->GetIPAddress(), targetname))
                    {
                        ShowActionReasonMenu(adminid, szAction,
                            std::begin(g_szReasonForPunishPlayer), std::end(g_szReasonForPunishPlayer),
                            std::bind(fnAction, target),
                            targetname
                            );
                    }
                }
                context.end(adminid);
            });
        }
        
        template<class CallbackFn = void(*)(std::string map)>
        void ShowSelectMapMenu(int adminid, const std::string &szAction, CallbackFn &&fnAction)
        {
            static_assert(std::is_invocable<CallbackFn, std::string>::value);
            util::ImMenu([adminid, szAction, fnAction](auto context) {
                context.begin("管理员装逼菜单 / Admin\n"
                              "选择进行" + szAction + "的原因");

                for(auto mapsv : mapmgr::GetServerMaps())
                {
                    std::string map(mapsv);
                    if(context.item(map, map))
                    {
                        ShowActionReasonMenu(adminid, szAction + "(" + map + ")", std::begin(g_szReasonForMap), std::end(g_szReasonForMap), std::bind(fnAction, map));
                    }
                }
                context.end(adminid);
            });
        }

        void EveryoneRTV()
        {
            std::ranges::for_each(sm::ranges::Players() | sm::ranges::Connected(), rtv::OnSayRTV);
        }

        void RestartGame()
        {
            ConVar *mp_restartgame = command::icvar->FindVar("mp_restartgame");
            mp_restartgame->SetValue("1");
        }

        template<class CallbackFn = void(*)(int amount)>
        void ShowSelectAmountMenu(int adminid, const std::string& szAction, CallbackFn&& fnAction, const std::string& szQuantifier = "个")
        {
            util::ImMenu([adminid, szAction, fnAction, szQuantifier](auto context) {
                context.begin("管理员装逼菜单 / Admin\n"
                    "选择要" + szAction + "的数量（单位：" + szQuantifier + "）");

                for (int amount : { 1, 5, 10, 20, 50, 100})
                {
                    if (context.item(std::to_string(amount), std::to_string(amount) + " " + szQuantifier))
                        fnAction(amount);
                }

                context.end(adminid);
                });
        }

        sm::coro::Task ShowGiveItemMenu(int adminid)
        {
            static std::vector<HyItemInfo> s_vecItemInfo;
            if (s_vecItemInfo.empty())
            {
                s_vecItemInfo = co_await itemown::GetCachedItemAvailableListAsync();
                sm::coro::RequestFrame();
            }
            util::ImMenu([adminid](auto context) {
                context.begin("管理员装逼菜单 / Admin\n"
                    "选择要赠送的道具");
                for (auto &&ii : s_vecItemInfo)
                {
                    if (context.item(ii.code, ii.name))
                    {
                        ShowSelectAmountMenu(adminid, "发道具(" + ii.name + ")", [adminid, ii](int amount) {
                            ShowSelectTargetPlayerMenu(adminid, "发道具(" + ii.name + std::to_string(amount) + ii.quantifier + ")", [ii, amount](int target) {
                                auto igpTarget = sm::IGamePlayerFrom(target);
                                HyDatabase().GiveItemBySteamID(igpTarget->GetSteam2Id(), ii.code, amount);
                                });
                            }, ii.quantifier);
                    }
                }
                context.end(adminid);
            });
            co_return;
        }

        void ShowGiveWeaponMenu(int adminid)
        {
            util::ImMenu([adminid](auto context) {
                context.begin("管理员装逼菜单 / Admin\n"
                    "选择要发的武器");
                for (const WeaponBuyInfo &ii : c_WeaponBuyList)
                {
                    if (context.item(ii.entity, ii.name))
                    {
                        ShowSelectTargetPlayerMenu(adminid, std::string() + "发武器(" + ii.name + ")", [ii](int target) {
                                auto igpTarget = sm::IGamePlayerFrom(target);
                                auto player = sm::ent_cast<CBasePlayer *>(target);
                                if(player && sm::IsPlayerAlive(igpTarget))
                                {
                                    auto ent = sm::sdktools::GivePlayerItem(player, ii.entity);
                                    if(ii.slot == CS_SLOT_KNIFE)
                                        sm::sdktools::EquipPlayerWeapon(player, ent);
                                }
                            });
                    }
                }
                context.end(adminid);
            });
        }

        int GetUserAccessLevel(int id)
        {
            if (adminsys->GetAdminFlag(sm::IGamePlayerFrom(id)->GetAdminId(), Admin_Reservation, Access_Real))
                return 4;
            if (adminsys->GetAdminFlag(sm::IGamePlayerFrom(id)->GetAdminId(), Admin_Generic, Access_Real))
                return 3;
            if (adminsys->GetAdminFlag(sm::IGamePlayerFrom(id)->GetAdminId(), Admin_Kick, Access_Real))
                return 2;
            return 0;
        }

        bool ShowAdminMenuPre(int adminid)
        {
            return adminsys->GetAdminFlag(sm::IGamePlayerFrom(adminid)->GetAdminId(), Admin_Generic, Access_Real);
        }

        void ShowAdminMenu(int adminid)
        {
            if(!ShowAdminMenuPre(adminid))
            {
                sm::PrintToChat(adminid, (std::string() + " \x05[死神CS社区]\x01 您不是管理员，不能装逼。").c_str());
                return;
            }

            util::ImMenu([adminid](auto context){
                context.begin("管理员装逼菜单 / Admin");

                AdminId aid = sm::IGamePlayerFrom(adminid)->GetAdminId();
                if (adminsys->GetAdminFlag(aid, Admin_Kick, Access_Real)) // c
                {
                    if (context.item("warn", "警告 / Warn"))
                        ShowSelectTargetPlayerMenu(adminid, "警告", [](int target) { return true; }, [adminid](int id) { return GetUserAccessLevel(adminid) >= GetUserAccessLevel(id); });
                    if (context.item("kill", "处死 / Kill"))
                        ShowSelectTargetPlayerMenu(adminid, "处死", [](int target) { return sm::sdktools::ForcePlayerSuicide(sm::ent_cast<CBasePlayer *>(target)), true; }, [adminid](int id) { return GetUserAccessLevel(adminid) >= GetUserAccessLevel(id) && sm::IsPlayerAlive(id); });
                    if (context.item("kick", "踢出 / Kick"))
                        ShowSelectTargetPlayerMenu(adminid, "踢出", [](int target) { return KickAndBan(target, 30), true; }, [adminid](int id) { return GetUserAccessLevel(adminid) >= GetUserAccessLevel(id); });
                    if (context.item("respawn", "复活 / Respawn"))
                        ShowSelectTargetPlayerMenu(adminid, "复活", [](int target) { return sm::cstrike::CS_RespawnPlayer(sm::ent_cast<CBasePlayer *>(target)), true; }, [adminid](int id) { return GetUserAccessLevel(adminid) >= GetUserAccessLevel(id); });
                    if (context.item("teamct", "处死并传送至CT / Team CT"))
                        ShowSelectTargetPlayerMenu(adminid, "传送至CT", [](int target) {
                        auto cbase = sm::ent_cast<CBasePlayer *>(target);
                        if (sm::IsPlayerAlive(cbase))
                            sm::sdktools::ForcePlayerSuicide(cbase);
                        return tools::SetTeam(sm::CBaseEntityFrom(target), CS_TEAM_CT), true;
                            }, [adminid](int id) { return tools::GetTeam(sm::id2cbase(id)) != CS_TEAM_CT && GetUserAccessLevel(adminid) >= GetUserAccessLevel(id); });
                    if (context.item("teamt", "处死并传送至TR / Team TR"))
                        ShowSelectTargetPlayerMenu(adminid, "传送至TR", [](int target) {
                        auto cbase = sm::ent_cast<CBasePlayer *>(target);
                        if (sm::IsPlayerAlive(cbase))
                            sm::sdktools::ForcePlayerSuicide(cbase);
                        return tools::SetTeam(sm::CBaseEntityFrom(target), CS_TEAM_T), true;
                            }, [adminid](int id) { return tools::GetTeam(sm::id2cbase(id)) != CS_TEAM_T && GetUserAccessLevel(adminid) >= GetUserAccessLevel(id); });

                    if (context.item("teamspec", "处死并传送至观察者 / Team Spec"))
                        ShowSelectTargetPlayerMenu(adminid, "传送至观察者", [](int target) {
                        auto cbase = sm::ent_cast<CBasePlayer *>(target);
                        if (tools::GetTeam(cbase) == 0)
                            sm::FakePlayerCommand(sm::id2edict(target), "joingame");
                        else
                            sm::sdktools::ForcePlayerSuicide(cbase);
                        return sm::ChangeClientTeam(sm::IGamePlayerFrom(target), CS_TEAM_SPECTATOR), true;
                            }, [adminid](int id) { return tools::GetTeam(sm::id2cbase(id)) != CS_TEAM_SPECTATOR && GetUserAccessLevel(adminid) >= GetUserAccessLevel(id); });

                    if (context.item("everyone_rtv", "开启RTV菜单 / Everyone RTV"))
                        ShowActionReasonMenu(adminid, "开启RTV菜单", std::begin(g_szReasonForMap), std::end(g_szReasonForMap), EveryoneRTV);
                }

                if (adminsys->GetAdminFlag(aid, Admin_Generic, Access_Real)) // b
                {
                    if (context.item("restart", "刷新服务器 / Restart"))
                        ShowActionReasonMenu(adminid, "刷新服务器", std::begin(g_szReasonForMap), std::end(g_szReasonForMap), RestartGame);
                    if (context.item("terminate", "结束回合 / Terminate"))
                        ShowActionReasonMenu(adminid, "结束回合", std::begin(g_szReasonForMap), std::end(g_szReasonForMap), [] { sm::cstrike::CS_TerminateRound(6.0f, CSRoundEnd_Draw); });
                    if (context.item("changelevel", "强制换图 / Changelevel"))
                        ShowSelectMapMenu(adminid, "强制换图", mapmgr::DelayedChangeLevel);

                    // TODO : ban
                    context.disabled();
                    context.item("ban", "全服封禁 / Ban");
                }

                if (adminsys->GetAdminFlag(aid, Admin_Reservation, Access_Real)) // a
                {
                    if (context.item("give_item", "发道具 / Give Item"))
                        ShowGiveItemMenu(adminid);
                    if (context.item("give_wpn", "发枪 / Give Weapon"))
                        ShowGiveWeaponMenu(adminid);
                    if (context.item("disarm", "缴枪 / Disarm"))
                        ShowSelectTargetPlayerMenu(adminid, "缴枪", [](int target) { return tools::RemoveAllWeapons(sm::ent_cast<CBasePlayer *>(target)), true; });
                }
                context.end(adminid);
            });
        }
    }
}