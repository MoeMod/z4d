
#include "extension.h"
#include "gameplay.h"
#include "util/smhelper.h"
#include "sm/sourcemod.h"

#include "itemown.h"

#include "HyDatabase.h"

#include <array>
#include <future>
#include <bitset>

namespace gameplay {
    namespace itemown {

        std::array<std::future<void>, SM_MAXPLAYERS+1> g_PlayerItemFuture;
        std::array<std::vector<HyUserOwnItemInfo>, SM_MAXPLAYERS+1> g_PlayerCachedItem;
        std::array<float, SM_MAXPLAYERS+1> g_PlayerCachedTime;
        std::bitset<SM_MAXPLAYERS+1> g_bitsPlayerCacheValid;

        void ShowItemMenuCached(int id)
        {
            auto menu = util::MakeMenu([](IBaseMenu* menu, int id, unsigned int item) {
                // TODO : use Item
                const char *code = menu->GetItemInfo(item, nullptr);
            });

            menu->RemoveAllItems();
            menu->SetDefaultTitle("我的道具 / Items");

            for(const auto &[item, amount] : g_PlayerCachedItem[id])
                menu->AppendItem(item.code.c_str(), (item.name + " " + std::to_string(amount) + item.quantifier).c_str());

            menu->SetMenuOptionFlags(menu->GetMenuOptionFlags() | MENUFLAG_BUTTON_EXIT);
            menu->Display(id, MENU_TIME_FOREVER);
        }

        void ShowItemOwnMenu(int id)
        {
            if(!g_bitsPlayerCacheValid.test(id) || sm::GetGameTime() < g_PlayerCachedTime[id] + 60)
                ShowItemMenuCached(id);

            std::string steamid = sm::IGamePlayerFrom(id)->GetSteam2Id();
            g_PlayerItemFuture[id] = std::async(std::launch::async, [id, steamid = std::move(steamid)]{
                auto item = HyDatabase().QueryUserOwnItemInfoBySteamID(steamid);

                std::sort(item.begin(), item.end(), [](const HyUserOwnItemInfo &a, const HyUserOwnItemInfo & b){
                    return a.amount > b.amount;
                });

                gameplay::RunOnMainThread([id, item = std::move(item)]{
                    g_PlayerCachedItem[id] = item;
                    g_PlayerCachedTime[id] = sm::GetGameTime();
                    g_bitsPlayerCacheValid.set(id, true);
                    ShowItemMenuCached(id);
                });
            });
        }

        void OnClientInit(int id)
        {
            g_PlayerItemFuture[id] = {};
            g_PlayerCachedItem[id] = {};
            g_PlayerCachedTime[id] = std::numeric_limits<float>::min();
            g_bitsPlayerCacheValid.set(id, false);
        }
    }
}

