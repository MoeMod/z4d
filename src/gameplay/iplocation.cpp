//
// Created by 梅峰源 on 2020-03-09.
//

#include "extension.h"
#include "sm/sourcemod.h"
#include "iplocation.h"
#include "util/qqwry.h"
#include "util/colorchat.h"

#include <vector>
#include <future>
#include <algorithm>
#include <util/smhelper.h>

namespace gameplay {
    namespace iplocation {

        std::string GetIPAddress(const std::string &ip) noexcept try
        {
            CQQWry qqwry("QQWry.dat");
            std::string country, area;
            qqwry.GetAddressByIp(ip.c_str(), country, area);
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
            auto location = GetIPAddress(ip);
            return location.empty() ?
                "\x04[Thanatos Zone]\x01 Welcome \x02" + name :
                "\x04[Thanatos Zone]\x01 Welcome \x02" + name + "\x01 from [" + location + "]";
        }

        // 以下函数在游戏主线程被调用：
        std::vector<std::future<std::string>> g_vecCachedWelcomeMessage;

        void OnClientInit(int id)
        {
            auto gp = sm::IGamePlayerFrom(id);
            // 注意：固定绑定std::string以免字符串提前析构
            std::string name = gp->GetName();
            std::string ip = gp->GetIPAddress();
            // 异步执行查IP地址操作
            g_vecCachedWelcomeMessage.emplace_back(std::async(std::launch::async, MakeWelcomeMessage, std::move(name), std::move(ip)));
        }

        void CheckWelcomeMessage()
        {
            static std::shared_ptr<ITimer> g_CheckWelcomeMessageTask = nullptr;
            // 将数组分为两半，已经准备好的部分放在后面一半
            auto iter = std::partition(g_vecCachedWelcomeMessage.begin(), g_vecCachedWelcomeMessage.end(), [](const std::future<std::string> &fut){ return !fut.valid(); });
            // 把已经处理好的显示出来
            std::for_each(iter, g_vecCachedWelcomeMessage.end(), [](std::future<std::string> &fut) {
                std::string val = fut.get();
                sm::PrintToChatAll(val.c_str());
            });
            // 去掉已经处理好的
            g_vecCachedWelcomeMessage.erase(iter, g_vecCachedWelcomeMessage.end());
            // 隔1秒后继续查询
            g_CheckWelcomeMessageTask = util::SetTask(1.0f, CheckWelcomeMessage);
        }

        void Init()
        {
            CheckWelcomeMessage();
        }
    }

}