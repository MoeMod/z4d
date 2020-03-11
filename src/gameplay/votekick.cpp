//
// Created by 梅峰源 on 2020-03-11.
//

#include "extension.h"
#include "votekick.h"
#include "util/smhelper.h"
#include "tools.h"

#include <string>
#include <vector>
#include <sm/sourcemod.h>
#include <numeric>
#include <bitset>
#include <memory>

namespace gameplay {
    namespace votekick {

        struct CurrentVote_t {
            int target = 0;
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
            g_pCurrentVote->approved.set(id, true);
            g_pCurrentVote->refused.set(id, false);
            g_pCurrentVote->abstained.set(id, false);
            auto menu = util::MakeMenu([](IBaseMenu* menu, int id, unsigned int item) {
                g_pCurrentVote->approved.set(id, item == 0);
                g_pCurrentVote->refused.set(id, item == 1);
                g_pCurrentVote->abstained.set(id, item == 2);
            });

            menu->RemoveAllItems();
            menu->SetDefaultTitle((std::string() + "投票踢出 / Vote Kick" + "\n是否赞成将" + sm::GetClientName(sm::IGamePlayerFrom(g_pCurrentVote->target)) + "踢出？").c_str());

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
                " \x02" + sm::GetClientName(sm::IGamePlayerFrom(g_pCurrentVote->target)) + "被强制踢出了").c_str());

                int time = 30;
                auto pPlayer = playerhelpers->GetGamePlayer(g_pCurrentVote->target);
                engine->ServerCommand((std::string() + "banid " + std::to_string(time) + " " + pPlayer->GetAuthString() + "\n").c_str());
                engine->ServerCommand((std::string() + "addip " + std::to_string(time) + " " + pPlayer->GetIPAddress() + "\n").c_str());
                engine->ServerCommand("writeip");
                engine->ServerCommand("writeid");
                gamehelpers->AddDelayedKick(g_pCurrentVote->target, pPlayer->GetUserId(), "[死神CS社区] Kicked : 您被强制踢出了");
            }
            else
            {
                sm::PrintToChatAll((std::string() + " \x05[死神CS社区]\x01 根据投票结果(" +
                std::to_string(g_pCurrentVote->approved.count()) + "赞成|" +
                std::to_string(g_pCurrentVote->refused.count()) + "反对|" +
                std::to_string(g_pCurrentVote->abstained.count()) + "弃权)" +
                " \x02" + sm::GetClientName(sm::IGamePlayerFrom(g_pCurrentVote->target)) + "没有被踢出").c_str());
            }
            g_pCurrentVote = nullptr;
            g_VoteEndTimer = nullptr;
        }

        void StartVote(int voter, int target)
        {
            // TODO : decrease tz_voteban
            g_pCurrentVote = std::make_unique<CurrentVote_t>();
            g_pCurrentVote->target = target;
            g_pCurrentVote->voter = voter;

            g_pCurrentVote->approved.set(voter, true);
            g_pCurrentVote->refused.set(target, true);

            sm::PrintToChatAll((std::string() + " \x05[死神CS社区]\x01 \x02" + sm::GetClientName(sm::IGamePlayerFrom(voter)) + "\x01 发起投票踢出 \x02" + sm::GetClientName(sm::IGamePlayerFrom(target))).c_str());

            for(int id = 1; id <= playerhelpers->GetMaxClients(); ++id)
            {
                auto igp = sm::IGamePlayerFrom(id);
                if(!igp || !igp->IsConnected())
                    continue;
                ShowJoinVoteMenu(id);
            }

            g_VoteEndTimer = util::SetTask(10.0f, Task_VoteEnd);
        }

        bool Show_StartVoteMenu(int voter)
        {
            assert(voter);
            // voting now
            if(g_pCurrentVote)
                return false;

            std::vector<int> candidate;
            std::iota(candidate.begin(), candidate.end(), 1);

            for(int id = 1; id <= playerhelpers->GetMaxClients(); ++id)
            {
                auto igp = sm::IGamePlayerFrom(id);
                if(!igp || !igp->IsConnected())
                    continue;
                candidate.push_back(id);
            }


            auto menu = util::MakeMenu([voter, candidate](IBaseMenu* menu, int id, unsigned int item) {
                StartVote(voter, candidate[item]);
            });

            menu->RemoveAllItems();
            menu->SetDefaultTitle("投票踢出 / Vote Kick");

            for(int id : candidate)
            {
                menu->AppendItem(std::to_string(id).c_str(), ItemDrawInfo(sm::GetClientName(sm::IGamePlayerFrom(id))));
            }

            menu->SetMenuOptionFlags(menu->GetMenuOptionFlags() | MENUFLAG_BUTTON_EXIT);
            menu->Display(voter, MENU_TIME_FOREVER);
            return true;
        }
    }
}