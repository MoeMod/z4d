//
// Created by 梅峰源 on 2020-05-05.
//

#include "extension.h"
#include "admin.h"
#include "sm/sourcemod.h"
#include "sm/sdktools.h"
#include "sm/cstrike.h"
#include "tools.h"
#include "rtv.h"
#include "mapmgr.h"

#include "util/ImMenu.hpp"

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
                sm::PrintToChatAll((std::string() + " \x05[死神CS社区]\x01 管理员" + adminname + "因为" + reason + "使用权限\x02" + action + "\x01").c_str());
            else
                sm::PrintToChatAll((std::string() + " \x05[死神CS社区]\x01 管理员" + adminname + "因为" + reason + "把" + targetname + "进行" + action).c_str());
            // TODO : log to mysql
        }

        void ShowSelectTargetPlayerMenu(int adminid, const std::string &szAction, const std::function<void(int target)> &fnAction)
        {
            util::ImMenu([adminid, szAction, fnAction](util::ImMenuContext &&context){
                context.begin("管理员装逼菜单 / Admin\n"
                              "选择进行" + szAction + "的目标玩家");
                for(int target = 1; target <= playerhelpers->GetMaxClients(); ++target)
                {
                    auto igp = sm::IGamePlayerFrom(target);
                    if(!igp || !igp->IsConnected())
                        continue;

                    std::string targetname = igp->GetName();
                    if(context.item(igp->GetIPAddress(), targetname))
                    {
                        util::ImMenu([adminid, target, targetname, szAction, fnAction](util::ImMenuContext &&context){
                            context.begin("管理员装逼菜单 / Admin\n"
                                          "选择对" + targetname + "进行" + szAction + "的原因");
                            for(auto reason : g_szReasonForPunishPlayer)
                            {
                                if(context.item(reason, reason))
                                {
                                    if(auto igpAdmin = sm::IGamePlayerFrom(adminid); igpAdmin && igpAdmin->IsConnected())
                                    {
                                        if(auto igpTarget = sm::IGamePlayerFrom(target); igpTarget && igpTarget->IsConnected())
                                        {
                                            fnAction(target);
                                            LogAndPrintAdmin(adminid, szAction, reason, targetname);
                                        }
                                    }
                                }
                            }
                            context.end(adminid);
                        });
                    }
                }
                context.end(adminid);
            });
        }

        void ShowActionReasonMapMenu(int adminid, const std::string &szAction, const std::function<void()> &fnAction)
        {
            util::ImMenu([adminid, szAction, fnAction](util::ImMenuContext &&context) {
                context.begin("管理员装逼菜单 / Admin\n"
                              "选择进行" + szAction + "的原因");

                for(auto reason : g_szReasonForMap)
                {
                    if(context.item(reason, reason))
                    {
                        fnAction();
                        LogAndPrintAdmin(adminid, szAction, reason);
                    }
                }

                context.end(adminid);
            });
        }

        void ShowSelectMapMenu(int adminid, const std::string &szAction, const std::function<void(const std::string &map)> &fnAction)
        {
            util::ImMenu([adminid, szAction, fnAction](util::ImMenuContext &&context) {
                context.begin("管理员装逼菜单 / Admin\n"
                              "选择进行" + szAction + "的原因");

                for(auto mapsv : mapmgr::GetServerMaps())
                {
                    std::string map(mapsv);
                    if(context.item(map, map))
                    {
                        ShowActionReasonMapMenu(adminid, szAction + "(" + map + ")", std::bind(fnAction, map));
                    }
                }
                context.end(adminid);
            });
        }

        void EveryoneRTV()
        {
            for(int id = 1; id <= playerhelpers->GetMaxClients(); ++id)
            {
                auto igp = sm::IGamePlayerFrom(id);
                if(!igp || !igp->IsConnected())
                    continue;

                rtv::OnSayRTV(id);
            }
        }

        void ChangelevelTo(const std::string &map)
        {
            static std::shared_ptr<void> g_ChangelevelTask;
            g_ChangelevelTask = util::SetTask(5.0, [map]{ engine->ServerCommand(("changelevel " + map + "\n").c_str()); g_ChangelevelTask = nullptr; });
        }

        void ShowAdminMenu(int adminid)
        {
            if(!adminsys->GetAdminFlag(adminid, Admin_Generic, Access_Real))
            {
                sm::PrintToChat(adminid, (std::string() + " \x05[死神CS社区]\x01 您不是管理员，不能装逼。").c_str());
                return;
            }

            util::ImMenu([adminid](auto &&context){
                context.begin("管理员装逼菜单 / Admin");

                if(context.item("warn", "警告 / Warn"))
                    ShowSelectTargetPlayerMenu(adminid, "警告", [](int target) { return true; });
                if(context.item("kill", "处死 / Kill"))
                    ShowSelectTargetPlayerMenu(adminid, "处死", [](int target) { return sm::sdktools::CommitSuicide(sm::CBaseEntityFrom(target)), true; });
                if(context.item("kick", "踢出 / Kick"))
                    ShowSelectTargetPlayerMenu(adminid, "踢出", [](int target) { return KickAndBan(target, 30), true; });

                // TODO : ban
                context.disabled();
                context.item("ban", "全服封禁 / Ban");

                if(context.item("respawn", "复活 / Respawn"))
                    ShowSelectTargetPlayerMenu(adminid, "复活", [](int target) { return sm::cstrike::CS_RespawnPlayer(sm::CBaseEntityFrom(target)), true; });
                if(context.item("teamct", "传送至CT / Team CT"))
                    ShowSelectTargetPlayerMenu(adminid, "传送至CT", [](int target) { return tools::SetTeam(sm::CBaseEntityFrom(target), CS_TEAM_CT), true; });
                if(context.item("teamt", "传送至TR / Team TR"))
                    ShowSelectTargetPlayerMenu(adminid, "传送至TR", [](int target) { return tools::SetTeam(sm::CBaseEntityFrom(target), CS_TEAM_T), true; });
                if(context.item("teamspec", "传送至观察者 / Team Spec"))
                    ShowSelectTargetPlayerMenu(adminid, "传送至观察者", [](int target) { return tools::SetTeam(sm::CBaseEntityFrom(target), CS_TEAM_SPECTATOR), true; });

                // TODO : admin give
                context.disabled();
                context.item("give", "发道具 / Give");

                if(context.item("restart", "刷新服务器 / Restart"))
                    ShowActionReasonMapMenu(adminid, "刷新服务器", []{ engine->ServerCommand("sv_restart 1"); });
                if(context.item("terminate", "结束回合 / Terminate"))
                    ShowActionReasonMapMenu(adminid, "结束回合", []{ sm::cstrike::CS_TerminateRound(1.0f, CSRoundEnd_Draw); });

                if(context.item("changelevel", "强制换图 / Changelevel"))
                    ShowSelectMapMenu(adminid, "强制换图", ChangelevelTo);

                if(context.item("everyone_rtv", "开启RTV菜单 / Everyone RTV"))
                    ShowActionReasonMapMenu(adminid, "开启RTV菜单", EveryoneRTV);

                context.end(adminid);
            });
        }
    }
}