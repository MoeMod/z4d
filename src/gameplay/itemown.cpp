
#include "extension.h"
#include "gameplay.h"
#include "util/ImMenu.hpp"
#include "sm/sourcemod.h"
#include "sm/interop.h"

#include "itemown.h"
#include "item_grenadepack.h"
#include "votekick.h"
#include "votemap.h"

#include "HyDatabase.h"

#include <array>
#include <map>
#include <future>
#include <bitset>
#include <sm/coro.h>

namespace gameplay {
    namespace itemown {

        ItemStatus ItemSelectPre(int id, const HyUserOwnItemInfo& ii)
        {
            ItemStatus is = ItemStatus::Disabled;
            is = std::max(is, ItemSelectPre_GrenadePack(id, ii));
            is = std::max(is, ItemSelectPre_VoteKick(id, ii));
            is = std::max(is, ItemSelectPre_VoteMap(id, ii));
            // TODO : sm api
            return is;
        }

        void ItemSelectPost(int id, const HyUserOwnItemInfo& ii)
        {
            ItemSelectPost_GrenadePack(id, ii);
            ItemSelectPost_VoteKick(id, ii);
            ItemSelectPost_VoteMap(id, ii);
            // TODO : sm api
        }

        std::array<std::map<std::string, HyUserOwnItemInfo>, SM_MAXPLAYERS+1> g_PlayerCachedItem;
        std::array<float, SM_MAXPLAYERS+1> g_PlayerCachedTime;
        std::bitset<SM_MAXPLAYERS+1> g_bitsPlayerCacheValid;
        std::shared_future<std::vector<HyItemInfo>> g_futureAllPlayerInfo;

        void ShowItemMenuCached(int id)
        {
            std::vector<std::pair<std::reference_wrapper<const HyUserOwnItemInfo>, ItemStatus>> vecShowItems;
            for(const auto &[code, ii] : g_PlayerCachedItem[id])
            {
                vecShowItems.emplace_back(std::cref(ii), ItemSelectPre(id, ii));
            }
            std::partition(vecShowItems.begin(), vecShowItems.end(), [id](const auto &iir) { return iir.second == ItemStatus::Available; });

            util::ImMenu([id, vecShowItems](util::ImMenuContext && context) {
                context.begin("我的道具 / Items");
                for(auto &&iir : vecShowItems)
                {
                    const auto &ii = iir.first.get();
                    const auto &[item, amount] = ii;
                    ItemStatus is = iir.second;

                    if(is == ItemStatus::Hidden)
                        continue;
                    
                    is == ItemStatus::Disabled ? context.disabled() : context.enabled();
                    if(context.item(item.code, (item.name + " " + std::to_string(amount) + item.quantifier)))
                    {
                        ItemSelectPost(id, ii);
                    }
                }
                context.end(id);
            });
        }

        sm::coro::Task CachePlayerItem(int id)
        {
            try{
                std::string steamid = sm::IGamePlayerFrom(id)->GetSteam2Id();
                std::vector<HyUserOwnItemInfo> item = co_await HyDatabase().async_QueryUserOwnItemInfoBySteamID(steamid);

                std::sort(item.begin(), item.end(), [](const HyUserOwnItemInfo &a, const HyUserOwnItemInfo & b){
                    return a.amount > b.amount;
                });

                co_await sm::coro::RequestFrame();
                if(!sm::IsClientConnected(sm::IGamePlayerFrom(id)))
                    co_return;

                g_PlayerCachedItem[id].clear();
                for (auto& ii : item)
                    g_PlayerCachedItem[id].emplace(ii.item.code, ii);

                g_PlayerCachedTime[id] = sm::GetGameTime();
                g_bitsPlayerCacheValid.set(id, true);
                co_return;
            }
            catch(...) {

            }
        }

        sm::coro::Task ShowItemOwnMenu(int id)
        {
            if(!g_bitsPlayerCacheValid.test(id) || sm::GetGameTime() < g_PlayerCachedTime[id] + 60)
                ShowItemMenuCached(id);

            co_await CachePlayerItem(id);
            ShowItemMenuCached(id);
        }

        void OnClientInit(int id)
        {
            g_PlayerCachedItem[id] = {};
            g_PlayerCachedTime[id] = {};
            g_bitsPlayerCacheValid.set(id, false);
        }

        sm::coro::Future<std::vector<HyItemInfo>> GetCachedItemAvailableListAsync()
        {
            static std::vector<HyItemInfo> cache;
            if(!cache.empty())
                co_return cache;
            std::vector<HyItemInfo> res = co_await HyDatabase().async_AllItemInfoAvailable();
            co_await sm::coro::RequestFrame();
            cache = std::move(res);
            co_return cache;
        }

        bool ItemConsume(int id, const std::string &code, unsigned amount)
        {
            assert(amount > 0);

            std::string steamid = sm::IGamePlayerFrom(id)->GetSteam2Id();
            bool result = HyDatabase().ConsumeItemBySteamID(steamid, code, amount);

            if(result)
            {
                g_bitsPlayerCacheValid.set(id, false);
                CachePlayerItem(id);
            }
            return result;
        }

        void async_ItemConsume(int id, const std::string& code, unsigned amount, std::function<void(bool)> fn)
        {
            assert(amount > 0);
            std::string steamid = sm::IGamePlayerFrom(id)->GetSteam2Id();
            HyDatabase().async_ConsumeItemBySteamID(steamid, code, amount, [id, fn](bool result) {
                if (result)
                {
                    sm::RequestFrame([id] {
                        g_bitsPlayerCacheValid.set(id, false);
                        CachePlayerItem(id);
                        });
                }
                fn(result);
            });
        }

        // typedef ItemConsumeCallback = function void (int id, const char[] code, int sub_amount, bool success, any data);
        // native void x_item_consume_async(int id, const char[] code, int sub_amount, ItemConsumeCallback fn, any data = 0);
        cell_t x_item_consume_async(IPluginContext* pContext, const cell_t* params)
        {
            auto args = sm::interop::params2tuple<
                int, std::string, int, std::function<void(int, std::string, int, bool, cell_t)>, cell_t
            >(pContext, params);
            const auto &[id, code, amount, cb, data] = args;
	        if ((id < 1) || (id > playerhelpers->GetMaxClients()))
		        return pContext->ThrowNativeError("Client index %d is invalid", id);
	        if(amount <= 0)
		        return pContext->ThrowNativeError("x_item_consume_async: amount %d should be > 0", amount);

            async_ItemConsume(id, code, amount, [args](bool success) {
                const auto& [id, code, amount, cb, data] = args;
                sm::RequestFrame(cb, id, code, amount, success, data);
            });
            return 0;
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
                CachePlayerItem(id);
            }
            return result;
        }

        void async_ItemGive(int id, const std::string& code, unsigned amount, std::function<void(bool)> fn)
        {
            assert(amount > 0);
            std::string steamid = sm::IGamePlayerFrom(id)->GetSteam2Id();
            HyDatabase().async_GiveItemBySteamID(steamid, code, amount, [id, fn](bool result) {
                if (result)
                {
                    sm::RequestFrame([id] {
                        g_bitsPlayerCacheValid.set(id, false);
                        CachePlayerItem(id);
                        });
                }
                fn(result);
            });
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

        // typedef ItemGiveCallback = function void (int id, const char[] code, int add_amount, bool success, any data);
        // native void x_item_give_async(int id, const char[] code, int add_amount, ItemConsumeCallback fn, any data = 0);
        cell_t x_item_give_async(IPluginContext* pContext, const cell_t* params)
        {
            auto args = sm::interop::params2tuple<
                int, std::string, int, std::function<void(int, std::string, int, bool, cell_t)>, cell_t
            >(pContext, params);
            const auto& [id, code, amount, cb, data] = args;
	        if ((id < 1) || (id > playerhelpers->GetMaxClients()))
		        return pContext->ThrowNativeError("Client index %d is invalid", id);
	        if(amount <= 0)
		        return pContext->ThrowNativeError("x_item_give_async: amount %d should be > 0", amount);

            async_ItemGive(id, code, amount, [args](bool success) {
                const auto& [id, code, amount, cb, data] = args;
                sm::RequestFrame(cb, id, code, amount, success, data);
                });
            return 0;
        }

        int ItemGet(int id, const std::string &code, bool use_cache)
        {
            if(use_cache)
            {
                if (g_bitsPlayerCacheValid.test(id))
                {
                    if (auto iter = g_PlayerCachedItem[id].find(code); iter != g_PlayerCachedItem[id].end())
                        return iter->second.amount;
                    return 0;
                }
                else
                {
                    CachePlayerItem(id);
                    return 0;
                }
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

