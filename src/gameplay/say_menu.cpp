//
// Created by 梅峰源 on 2020-03-15.
//

#include "extension.h"
#include "say_menu.h"
#include "util/ImMenu.hpp"
#include "sm/sourcemod.h"
#include "sm/coro.h"

#include "qqlogin.h"
#include "rtv.h"
#include "votekick.h"
#include "votemap.h"
#include "itemown.h"
#include "itemshop.h"
#include "admin.h"

#include <random>
#include <functional>

namespace gameplay {
    namespace say_menu {
        static std::shared_ptr<ITimer> g_taskShowMainMenuHint;

        constexpr const char* c_Hints[] = {
            " \x05[死神CS社区]\x01 提示:您可以按\x02[F4]\x01键来打开社区主菜单(投票/道具/注册)",
            " \x05[死神CS社区]\x01 提示:请加入我们的官方 QQ 群\x02 900000025 \x01(中间6个0)。",
            " \x05[死神CS社区]\x01 提示:投诉/开黑 Yy频道：\x02 750046 \x01。"
        }; 

        sm::coro::Task Co_ShowMainMenuHint()
        {
            std::random_device rd;
            std::uniform_int_distribution<std::size_t> rg(0, std::extent<decltype(c_Hints)>::value - 1);
            while (1)
            {
                co_await sm::coro::CreateTimer(std::uniform_real_distribution<float>(30, 120)(rd));
                sm::PrintToChatAll(c_Hints[rg(rd)]);
            }
        }

        void Init()
        {
            Co_ShowMainMenuHint();
        }

        static const std::tuple<const char*, std::function<bool(int id)>, std::function<void(int id)>> funclist[] = {
                    {"我的账号 / Account", std::bind(std::true_type()), qqlogin::ShowAccountMenu },
                    {"我的道具 / Item", std::bind(std::true_type()), itemown::ShowItemOwnMenu },
                    {"钦点地图 / RTV", std::bind(std::true_type()), rtv::OnSayRTV },
                    {"投票踢人(用券) / VoteKick", std::bind(itemown::ItemGet, std::placeholders::_1, "tz_votekick", true), votekick::Show_StartVoteMenu },
                    {"投票换图(用券) / VoteMap", std::bind(itemown::ItemGet, std::placeholders::_1, "tz_votemap", true), votemap::Show_StartVoteMenu },
                    {"道具商店 / ItemShop", std::bind(std::true_type()), itemshop::Show_ItemShopMenu },
                    {"管理员装逼菜单 / Admin", admin::ShowAdminMenuPre, admin::ShowAdminMenu }
        };

        void ShowMainMenu(int id)
        {
            util::ImMenu([id](auto&& context) {
                context.begin("死神社区主菜单 / Thanatos Main");
                for (const auto& [name, pre, post] : funclist)
                {
                    pre(id) ? context.enabled() : context.disabled();
                    if (context.item(name, name))
                        post(id);
                }
                context.end(id);
            });
        }
    }
}