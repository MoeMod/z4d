//
// Created by 梅峰源 on 2020-03-09.
//

#include "extension.h"
#include "sm/sourcemod.h"
#include "sm/coro.h"
#include "sm/ranges.h"
#include "iplocation.h"
#include "qqlogin.h"
#include "util/qqwry.h"
#include "util/Encode.h"

#include <vector>
#include <array>
#include <future>
#include <algorithm>
#include <util/smhelper.h>

namespace gameplay {
    namespace iplocation {

        std::string GetIPLocation(std::string ip) noexcept try
        {
            // 放到游戏根目录
            CQQWry qqwry("QQWry.dat");
            std::string country, area;
            // 去掉ip后面的端口号（不知道为什么会有...）
            ip.erase(std::find(ip.begin(), ip.end(), ':'), ip.end());
            qqwry.GetAddressByIp(ip.c_str(), country, area);
            country = GBK_To_UTF8(country);
            area = GBK_To_UTF8(area);
            if(country == " CZ88.NET")
                country.clear();
            if(area == " CZ88.NET")
                area.clear();
            return country + " " + area;
        } catch(...)
        {
            return {};
        }

        std::string MakeWelcomeMessage(const std::string &name, const std::string &ip, char tag_color, std::string tag)
        {
            if (!tag.empty())
                tag = std::string() + tag_color + "【" + std::move(tag) + "】" + "\x01";
            auto location = GetIPLocation(ip);
            return location.empty() ?
                " \x05[死神CS社区]\x01 欢迎" + tag + " \x02" + name + "\x01 进入服务器":
                " \x05[死神CS社区]\x01 欢迎" + tag + " \x02" + name + "\x01 来自 [" + location + "] 进入服务器";
        }

        sm::coro::Task ShowWelcomeMessage(int id)
        {
            co_await sm::coro::CreateTimer(1.0);
            if (auto gp = sm::ent_cast<IGamePlayer *>(id))
            {
                std::string name = gp->GetName();
                std::string ip = gp->GetIPAddress();
                char tagcolor = qqlogin::GetUserTagColor(id);
                auto tag = qqlogin::GetUserTag(id);

                auto msg = MakeWelcomeMessage(name, ip, tagcolor, tag);
                for(auto player : sm::ranges::Players() | sm::ranges::Connected())
                    sm::PrintToChatStr(player, msg);
            }
        }

        void OnClientPutInServer(int id)
        {
            iplocation::ShowWelcomeMessage(id);
        }

        void Init()
        {
            
        }
    }

}