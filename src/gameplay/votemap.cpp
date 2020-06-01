//
// Created by 梅峰源 on 2020-03-11.
//

#include "extension.h"
#include "votemap.h"
#include "util/smhelper.h"
#include "tools.h"
#include "mapmgr.h"

#include "itemown.h"
#include "HyDatabase.h"

#include <string>
#include <vector>
#include <sm/sourcemod.h>
#include <numeric>
#include <bitset>
#include <memory>

namespace gameplay {
    namespace itemown {
        ItemStatus ItemSelectPre_VoteMap(int id, const HyUserOwnItemInfo& ii)
        {
            if(ii.item.code == "tz_votemap")
            {
                return ItemStatus::Available;
            }
            return ItemStatus::Disabled;
        }

        void ItemSelectPost_VoteMap(int id, const HyUserOwnItemInfo& ii)
        {
            if(ii.item.code == "tz_votemap")
            {
                votemap::Show_StartVoteMenu(id);
            }
        }
    }

    namespace votemap {

        struct CurrentVote_t {
            std::string target = {};
            int voter = 0;
            std::bitset<SM_MAXPLAYERS + 1> approved;
            std::bitset<SM_MAXPLAYERS + 1> refused;
            std::bitset<SM_MAXPLAYERS + 1> abstained;
        };
        std::unique_ptr<CurrentVote_t> g_pCurrentVote;
        std::shared_ptr<ITimer> g_VoteEndTimer;


        void Init()
        {
            g_pCurrentVote = nullptr;
            g_VoteEndTimer = nullptr;
        }

        void ShowJoinVoteMenu(int id)
        {
            assert(g_pCurrentVote != nullptr);
            g_pCurrentVote->approved.set(id, false);
            g_pCurrentVote->refused.set(id, false);
            g_pCurrentVote->abstained.set(id, true);
            auto menu = util::MakeMenu([](IBaseMenu* menu, int id, unsigned int item) {
                g_pCurrentVote->approved.set(id, item == 0);
                g_pCurrentVote->refused.set(id, item == 1);
                g_pCurrentVote->abstained.set(id, item == 2);
            });

            menu->RemoveAllItems();
            menu->SetDefaultTitle((std::string() + "投票换图 / Vote Map" + "\n是否赞成更换地图为" + g_pCurrentVote->target + "？").c_str());

            menu->AppendItem("appr", ItemDrawInfo("我赞成 / Approve"));
            menu->AppendItem("refu", ItemDrawInfo("我反对 / Refuse"));
            menu->AppendItem("abst", ItemDrawInfo("我弃权 / Abstain"));

            menu->SetMenuOptionFlags(menu->GetMenuOptionFlags());
            menu->Display(id, 10);
        }

        void Task_VoteEnd()
        {
            assert(g_pCurrentVote != nullptr);
            if(g_pCurrentVote->approved.any() && g_pCurrentVote->approved.count() > g_pCurrentVote->refused.count() && g_pCurrentVote->abstained.count() <= playerhelpers->GetNumPlayers() / 2)
            {
                sm::PrintToChatAll((std::string() + " \x05[死神CS社区]\x01 根据投票结果(" +
                std::to_string(g_pCurrentVote->approved.count()) + "赞成|" +
                std::to_string(g_pCurrentVote->refused.count()) + "反对|" +
                std::to_string(g_pCurrentVote->abstained.count()) + "弃权)" +
                " \x01 即将更换地图为 \x02" + g_pCurrentVote->target + " \x01").c_str());

                mapmgr::DelayedChangeLevel(g_pCurrentVote->target);
            }
            else
            {
                sm::PrintToChatAll((std::string() + " \x05[死神CS社区]\x01 根据投票结果(" +
                                    std::to_string(g_pCurrentVote->approved.count()) + "赞成|" +
                                    std::to_string(g_pCurrentVote->refused.count()) + "反对|" +
                                    std::to_string(g_pCurrentVote->abstained.count()) + "弃权)" +
                                    " \x01 投票换图 \x02" + g_pCurrentVote->target + " \x01失败").c_str());
            }
            g_pCurrentVote = nullptr;
            g_VoteEndTimer = nullptr;
        }

        void StartVote(int voter, std::string_view target)
        {
            // decrease tz_votemap
            if(!itemown::ItemConsume(voter, "tz_votemap", 1))
            {
                sm::PrintToChat(voter, (std::string() + " \x05[死神CS社区]\x01 您没有投票换图券，不能使用投票换图功能。可以通过 QQ 群签到获得更多的道具。").c_str());
                return;
            }

            g_pCurrentVote = std::make_unique<CurrentVote_t>();
            g_pCurrentVote->target = target;
            g_pCurrentVote->voter = voter;

            sm::PrintToChatAll((std::string() + " \x05[死神CS社区]\x01 \x02" + sm::GetClientName(sm::IGamePlayerFrom(voter)) + "\x01 发起投票换图为 \x02" + g_pCurrentVote->target + " \x01").c_str());

            for(int id = 1; id <= playerhelpers->GetMaxClients(); ++id)
            {
                auto igp = sm::IGamePlayerFrom(id);
                if(!sm::IsClientConnected(igp))
                    continue;
                ShowJoinVoteMenu(id);
            }

            g_VoteEndTimer = sm::CreateTimer(10.0f, Task_VoteEnd);
        }

        bool Show_StartVoteMenu(int voter)
        {
            assert(voter);
            // voting now
            if(g_pCurrentVote)
                return false;

            // check tz_votemap
            if(!itemown::ItemGet(voter, "tz_votemap"))
            {
                sm::PrintToChat(voter, (std::string() + " \x05[死神CS社区]\x01 您没有投票换图券，不能使用投票换图功能。可以通过 QQ 群签到获得更多的道具。").c_str());
                return false;
            }

            std::vector<std::string_view> candidate = mapmgr::GetServerMaps();

            auto menu = util::MakeMenu([voter, candidate](IBaseMenu* menu, int id, unsigned int item) {
                StartVote(voter, candidate[item]);
            });

            menu->RemoveAllItems();
            menu->SetDefaultTitle("投票换图 / Vote Map");

            for(std::string_view map_sv : candidate)
            {
                std::string map(map_sv);
                menu->AppendItem(map.c_str(), ItemDrawInfo(map.c_str()));
            }

            menu->SetMenuOptionFlags(menu->GetMenuOptionFlags() | MENUFLAG_BUTTON_EXIT);
            menu->Display(voter, MENU_TIME_FOREVER);
            return true;
        }
    }
}