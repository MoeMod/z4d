//
// Created by 梅峰源 on 2020-03-15.
//

#include "extension.h"
#include "say_menu.h"
#include "util/smhelper.h"
#include "sm/sourcemod.h"

#include "qqlogin.h"
#include "rtv.h"
#include "votekick.h"
#include "itemown.h"
#include "admin.h"

#include <random>
#include <functional>

namespace gameplay {
    namespace say_menu {
        static std::shared_ptr<void> g_taskShowMainMenuHint;

        void Task_ShowMainMenuHint()
        {
            static std::random_device rd;

            sm::PrintToChatAll(" \x05[死神CS社区]\x01 提示:您可以按y键输入\x02menu\x01来打开主菜单(投票/道具/注册)");

            // 一段时间之后重新显示
            g_taskShowMainMenuHint = sm::CreateTimer(std::normal_distribution<float>(30, 120)(rd), Task_ShowMainMenuHint);
        }

        void Init()
        {
            g_taskShowMainMenuHint = sm::CreateTimer(60, Task_ShowMainMenuHint);
        }

        void ShowMainMenu(int id)
        {
            static const std::pair<const char *, std::function<void(int id)>> funclist[] = {
                    {"我的账号 / Account", qqlogin::ShowAccountMenu },
                    {"我的道具 / Item", itemown::ShowItemOwnMenu },
                    {"钦点地图 / RTV", rtv::OnSayRTV },
                    {"投票踢人 / VoteKick", votekick::Show_StartVoteMenu },
                    {"管理员装逼菜单 / Admin", admin::ShowAdminMenu }
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