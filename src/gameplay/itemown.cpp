
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

        void CachePlayerItem(int id, bool show_menu_on_cached)
        {
            std::string steamid = sm::IGamePlayerFrom(id)->GetSteam2Id();
            g_PlayerItemFuture[id] = std::async(std::launch::async, [id, steamid = std::move(steamid), show_menu_on_cached]{
                auto item = HyDatabase().QueryUserOwnItemInfoBySteamID(steamid);

                std::sort(item.begin(), item.end(), [](const HyUserOwnItemInfo &a, const HyUserOwnItemInfo & b){
                    return a.amount > b.amount;
                });

                gameplay::RunOnMainThread([id, item = std::move(item), show_menu_on_cached]{
                    g_PlayerCachedItem[id] = std::move(item);
                    g_PlayerCachedTime[id] = sm::GetGameTime();
                    g_bitsPlayerCacheValid.set(id, true);
                    if(show_menu_on_cached)
                        ShowItemMenuCached(id);
                });
            });
        }

        void ShowItemOwnMenu(int id)
        {
            if(!g_bitsPlayerCacheValid.test(id) || sm::GetGameTime() < g_PlayerCachedTime[id] + 60)
                ShowItemMenuCached(id);

            CachePlayerItem(id, true);
        }

        void OnClientInit(int id)
        {
            g_PlayerItemFuture[id] = {};
            g_PlayerCachedItem[id] = {};
            g_PlayerCachedTime[id] = {};
            g_bitsPlayerCacheValid.set(id, false);
        }

        bool ItemConsume(int id, const std::string &code, unsigned amount)
        {
            assert(amount > 0);

            std::string steamid = sm::IGamePlayerFrom(id)->GetSteam2Id();
            bool result = HyDatabase().ConsumeItemBySteamID(steamid, code, amount);

            if(result)
            {
                g_bitsPlayerCacheValid.set(id, false);
                CachePlayerItem(id, false);
            }
            return false;
        }

        cell_t x_item_consume(IPluginContext *pContext, const cell_t *params)
        {
            int id = params[1];

            if ((id < 1) || (id > playerhelpers->GetMaxClients()))
            {
                return pContext->ThrowNativeError("Client index %d is invalid", id);
            }

            char *code;
            pContext->LocalToString(params[2], &code);

            int amount = params[3];
            if(amount <= 0)
            {
                return pContext->ThrowNativeError("x_item_consume: amount %d should be > 0", amount);
            }

            return ItemConsume(id, code, amount);
        }

        bool ItemGive(int id, const std::string &code, unsigned amount)
        {
            assert(amount > 0);

            std::string steamid = sm::IGamePlayerFrom(id)->GetSteam2Id();
            bool result = HyDatabase().GiveItemBySteamID(steamid, code, amount);
            if(result)
            {
                g_bitsPlayerCacheValid.set(id, false);
                CachePlayerItem(id, false);
            }
            return false;
        }

        cell_t x_item_give(IPluginContext *pContext, const cell_t *params)
        {
            int id = params[1];

            if ((id < 1) || (id > playerhelpers->GetMaxClients()))
            {
                return pContext->ThrowNativeError("Client index %d is invalid", id);
            }

            char *code;
            pContext->LocalToString(params[2], &code);

            int amount = params[3];
            if(amount <= 0)
            {
                return pContext->ThrowNativeError("x_item_give: amount %d should be > 0", amount);
            }

            return ItemGive(id, code, amount);
        }

        int ItemGet(int id, const std::string &code, bool use_cache)
        {
            if(use_cache && g_bitsPlayerCacheValid.test(id))
            {
                auto iter = std::find_if(g_PlayerCachedItem[id].begin(), g_PlayerCachedItem[id].end(), [&code](const HyUserOwnItemInfo &ii){
                    return ii.item.code == code;
                });
                if(iter == g_PlayerCachedItem[id].end())
                    return 0;
                return iter->amount;
            }
            std::string steamid = sm::IGamePlayerFrom(id)->GetSteam2Id();
            return HyDatabase().GetItemAmountBySteamID(steamid, code);
        }

        cell_t x_item_get(IPluginContext *pContext, const cell_t *params)
        {
            int id = params[1];

            if ((id < 1) || (id > playerhelpers->GetMaxClients()))
            {
                return pContext->ThrowNativeError("Client index %d is invalid", id);
            }

            char *code;
            pContext->LocalToString(params[2], &code);

            return ItemGet(id, code, true);
        }
    }
}

