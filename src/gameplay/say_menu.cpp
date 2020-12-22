//
// Created by 梅峰源 on 2020-03-15.
//

#include "extension.h"
#include "say_menu.h"
#include "util/ImMenu.hpp"
#include "sm/sourcemod.h"
#include "sm/coro.h"

#include "rtv.h"
#include "admin.h"

#include <random>
#include <functional>

namespace gameplay {
    namespace say_menu {

        constexpr const char* c_Hints[] = {
            " \x05[死神CS社区]\x01 提示:您可以按\x02[F4]\x01键来打开社区主菜单(投票/道具/注册)",
            " \x05[死神CS社区]\x01 提示:请加入我们的官方 QQ 群\x02 900000025 \x01(中间6个0)。",
            " \x05[死神CS社区]\x01 提示:投诉/开黑 Yy频道：\x02 750046 \x01。"
        }; 

        sm::coro::Task Co_ShowMainMenuHint()
        {
            while (1)
            {
                for (const char* hint : c_Hints)
                {
                    co_await sm::coro::CreateTimer(30);
                    sm::PrintToChatAll(hint);
                }
            }
        }

        void Init()
        {
            Co_ShowMainMenuHint();
        }

        static const std::tuple<const char*, std::function<bool(int id)>, std::function<void(int id)>> funclist[] = {
                    {"钦点地图 / RTV", std::bind(std::true_type()), rtv::OnSayRTV },
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