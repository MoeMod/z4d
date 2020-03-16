#include "extension.h"
#include "gameplay.h"
#include "util/smhelper.h"
#include "sm/sourcemod.h"

#include "qqlogin.h"

#include "HyDatabase.h"

#include <array>
#include <future>

namespace gameplay {
    namespace qqlogin {

        std::array<HyUserAccountData, SM_MAXPLAYERS+1> g_PlayerAccountData;
        std::array<std::future<void>, SM_MAXPLAYERS+1> g_PlayerAccountFuture;

        // *** 以下函数异步进行 ***
        HyUserAccountData GetUserAccountData(const std::string & steamid) 
        {
            try {
                return HyDatabase().QueryUserAccountDataBySteamID(steamid);
            }
            catch (const InvalidUserAccountDataException & e)
            {
                return { 0, "", steamid };
            }
        }

        void FlushAccountData(int id, const std::string& steamid)
        {
            auto account = GetUserAccountData(steamid); // 耗时操作
            // 完成后回到主线程再更新记录
            gameplay::RunOnMainThread([id, account] {
                AdminId adm = adminsys->CreateAdmin(nullptr);
                adminsys->SetAdminFlags(adm, Access_Real, adminsys->ReadFlagString(account.access.c_str(), nullptr));
                sm::IGamePlayerFrom(id)->SetAdminId(adm, true);
                g_PlayerAccountData[id] = account;
            });
        }

        // *** 以下函数在主线程执行 ***

        void ShowAccountMenu(int id)
        {
            if (g_PlayerAccountData[id].qqid == 0)
            {
                const std::string steamid = sm::IGamePlayerFrom(id)->GetSteam2Id();
                g_PlayerAccountFuture[id] = std::async(std::launch::async, [id, steamid] {
                    int32_t gocode = HyDatabase().StartRegistrationWithSteamID(steamid);

                    gameplay::RunOnMainThread([id, gocode, steamid] {
                        if (!sm::IsClientConnected(sm::IGamePlayerFrom(id)))
                            return;

                        auto menu = util::MakeMenu([steamid](IBaseMenu* menu, int id, unsigned int item) {
                            g_PlayerAccountFuture[id] = std::async(std::launch::async, [id, steamid] {
                                using namespace std::chrono_literals;
                                FlushAccountData(id, steamid);
                                g_PlayerAccountFuture[id].wait_for(3s);
                                gameplay::RunOnMainThread(std::bind(ShowAccountMenu, id));
                            });
                        });

                        menu->RemoveAllItems();
                        menu->SetDefaultTitle((std::string() +
                            "SteamID : " + sm::IGamePlayerFrom(id)->GetSteam2Id() + "\n"
                            "权限 : " + g_PlayerAccountData[id].tag + "\n"
                            "请加入QQ群1326357 \n"
                            "在群中输入【" + std::to_string(gocode) + "】进行注册\n"
                            ).c_str());

                        menu->AppendItem("refresh", "刷新信息 / F5");

                        menu->SetMenuOptionFlags(menu->GetMenuOptionFlags() | MENUFLAG_BUTTON_EXIT);
                        menu->Display(id, MENU_TIME_FOREVER);
                    });
                });
            }
            else
            {
                auto menu = util::MakeMenu([](IBaseMenu* menu, int id, unsigned int item) {});

                menu->RemoveAllItems();
                menu->SetDefaultTitle((std::string() +
                    "SteamID : " + sm::IGamePlayerFrom(id)->GetSteam2Id() + "\n" +
                    "权限 : " + g_PlayerAccountData[id].tag + "\n" +
                    "绑定QQ : " + std::to_string(g_PlayerAccountData[id].qqid) + "\n"
                    ).c_str());

                menu->SetMenuOptionFlags(menu->GetMenuOptionFlags() | MENUFLAG_BUTTON_EXIT);
                menu->Display(id, MENU_TIME_FOREVER);
            }
        }

        const std::string& GetUserTag(int id)
        {
            return g_PlayerAccountData[id].tag;
        }

        void OnClientPreAdminCheck(int id)
        {
            // 注意转换成std::string以免出现野指针
            g_PlayerAccountData[id] = {};
            g_PlayerAccountFuture[id] = std::async(std::launch::async,
                FlushAccountData, id, std::string(playerhelpers->GetGamePlayer(id)->GetSteam2Id(false))
            );
        }

        void Init()
        {

        }
    }
}
