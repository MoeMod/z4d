//
// Created by 梅峰源 on 2020-03-15.
//

#include "extension.h"
#include "say_menu.h"
#include "util/smhelper.h"
#include "sm/sourcemod.h"

#include "rtv.h"
#include "votekick.h"

#include <random>
#include <functional>

namespace gameplay {
    namespace say_menu {

        void Task_ShowMainMenuHint()
        {
            static std::shared_ptr<void> g_taskShowMainMenuHint;
            static std::random_device rd;

            if(g_taskShowMainMenuHint != nullptr)
                sm::PrintToChatAll(" \x05[死神CS社区]\x01 提示：您可以按y键输入\x02menu\x01 来打开主菜单~~~");

            // 一段时间之后重新显示
            g_taskShowMainMenuHint = util::SetTask(std::normal_distribution<float>(30, 120)(rd), Task_ShowMainMenuHint);
        }

        void Init()
        {
            Task_ShowMainMenuHint();
        }

        void ShowMainMenu(int id)
        {
            static const std::pair<const char *, std::function<void(int id)>> funclist[] = {
                    {"投票换图 / RTV", rtv::OnSayRTV },
                    {"投票踢人 / VoteKick", votekick::Show_StartVoteMenu },
            };

            auto menu = util::MakeMenu([](IBaseMenu* menu, int id, unsigned int item) {
                funclist[item].second(id);
            });

            menu->RemoveAllItems();
            menu->SetDefaultTitle("死神社区主菜单 / Thanatos Main");

            for(const auto &[name, fn] : funclist)
            {
                menu->AppendItem(name, ItemDrawInfo(name));
            }

            menu->SetMenuOptionFlags(menu->GetMenuOptionFlags() | MENUFLAG_BUTTON_EXIT);
            menu->Display(id, MENU_TIME_FOREVER);
        }
    }
}