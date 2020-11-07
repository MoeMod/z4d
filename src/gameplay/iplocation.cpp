//
// Created by 梅峰源 on 2020-03-09.
//

#include "extension.h"
#include "sm/sourcemod.h"
#include "iplocation.h"
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

        std::string MakeWelcomeMessage(const std::string &name, const std::string &ip)
        {
            auto location = GetIPLocation(ip);
            return location.empty() ?
                " \x05[死神CS社区]\x01 欢迎\x02" + name + "\x01 进入服务器":
                " \x05[死神CS社区]\x01 欢迎\x02" + name + "\x01 来自 [" + location + "] 进入服务器";
        }

        void ShowWelcomeMessage(int id)
        {
            sm::CreateTimer(1.0, [id] {
                if (auto gp = sm::IGamePlayerFrom(id))
                {
                    std::string name = gp->GetName();
                    std::string ip = gp->GetIPAddress();

                    auto msg = MakeWelcomeMessage(std::move(name), std::move(ip));
                    sm::PrintToChatAllStr(msg);
                }
            });
        }

        void Init()
        {
            
        }
    }

}