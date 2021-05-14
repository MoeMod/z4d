#include "extension.h"
#include "gameplay.h"
#include "util/smhelper.h"
#include "sm/sourcemod.h"
#include "sm/coro.h"
#include "csgo_colorchat.h"

#include "qqlogin.h"

#include "HyDatabase.h"

#include <array>
#include <future>

namespace gameplay {
    namespace qqlogin {

        std::array<HyUserAccountData, SM_MAXPLAYERS+1> g_PlayerAccountData;

        sm::coro::Task async_FlushAccountData(int id, const std::string& steamid)
        {
            HyUserAccountData account{ 0, "", steamid };
            try {
                account = co_await HyDatabase().async_QueryUserAccountDataBySteamID(steamid);
            } catch(...) {}

            // 完成后回到主线程再更新记录
            co_await sm::coro::RequestFrame();

            g_PlayerAccountData[id] = account;
            if (!account.access.empty())
            {
                AdminId adm = adminsys->CreateAdmin(nullptr);
                adminsys->SetAdminFlags(adm, Access_Real, adminsys->ReadFlagString(account.access.c_str(), nullptr));
                sm::IGamePlayerFrom(id)->SetAdminId(adm, true);
            }
            co_return;
        }

        sm::coro::Task ShowAccountMenu(int id)
        {
            const std::string steamid = sm::IGamePlayerFrom(id)->GetSteam2Id();
            if (g_PlayerAccountData[id].qqid == 0)
            {
                int32_t gocode = co_await HyDatabase().async_StartRegistrationWithSteamID(steamid);
                co_await sm::coro::RequestFrame();

                if (!sm::IsClientConnected(sm::IGamePlayerFrom(id)))
                    co_return;

                auto menu = util::MakeMenu([steamid](IBaseMenu* menu, int id, unsigned int item) {
                    sm::coro::AsyncAwait(async_FlushAccountData(id, steamid), std::bind(ShowAccountMenu, id));
                });

                menu->RemoveAllItems();
                menu->SetDefaultTitle((std::string() +
                    "SteamID : " + sm::IGamePlayerFrom(id)->GetSteam2Id() + "\n"
                    "权限 : " + g_PlayerAccountData[id].tag + "\n"
                    "请加入QQ群900000025(中间6个0) \n"
                    "在群中输入【" + std::to_string(gocode) + "】进行注册\n"
                    ).c_str());

                menu->AppendItem("refresh", "刷新信息 / F5");

                menu->SetMenuOptionFlags(menu->GetMenuOptionFlags() | MENUFLAG_BUTTON_EXIT);
                menu->Display(id, MENU_TIME_FOREVER);
            }
            else
            {
                auto menu = util::MakeMenu([steamid](IBaseMenu* menu, int id, unsigned int item) {
                    sm::coro::AsyncAwait(async_FlushAccountData(id, steamid), std::bind(ShowAccountMenu, id));
                });

                menu->RemoveAllItems();
                menu->SetDefaultTitle((std::string() +
                    "SteamID : " + steamid + "\n" +
                    "权限 : " + g_PlayerAccountData[id].tag + "\n" +
                    "绑定QQ : " + std::to_string(g_PlayerAccountData[id].qqid) + "\n"
                    ).c_str());

                menu->AppendItem("refresh", "刷新信息 / F5");

                menu->SetMenuOptionFlags(menu->GetMenuOptionFlags() | MENUFLAG_BUTTON_EXIT);
                menu->Display(id, MENU_TIME_FOREVER);
            }
        }

        std::string GetUserTag(int id)
        {
            return g_PlayerAccountData[id].tag;
        }

        char GetUserTagColor(int id)
        {
            if (g_PlayerAccountData[id].access.find('a') != std::string::npos)
                return colorchat::lightpurple;
            if (g_PlayerAccountData[id].access.find('b') != std::string::npos)
                return colorchat::red;
            if (g_PlayerAccountData[id].access.find('c') != std::string::npos)
                return colorchat::orange;
            if (g_PlayerAccountData[id].access.find('o') != std::string::npos)
                return colorchat::lightorange;
            return colorchat::lightgreen2;
        }

        void OnClientPreAdminCheck(int id)
        {
            // 注意转换成std::string以免出现野指针
            g_PlayerAccountData[id] = {};
            async_FlushAccountData(id, playerhelpers->GetGamePlayer(id)->GetSteam2Id(false));
        }

        void Init()
        {

        }
    }
}
