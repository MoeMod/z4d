#pragma once

#include <string>
#include "HyDatabase.h"
#include "sm/coro.h"
#include <functional>

namespace gameplay {
    namespace itemown {

        enum class ItemStatus {
            Disabled = 0,
            Available = 1,
            Hidden = 2,
        };

        sm::coro::Task ShowItemOwnMenu(int id);
        void OnClientInit(int id);

        // 获取服务器道具列表（可能使用缓存），callback会在主线程执行（也可能立即执行）
        sm::coro::Future<std::vector<HyItemInfo>> GetCachedItemAvailableListAsync();

        bool ItemConsume(int id, const std::string &code, unsigned amount);
        cell_t x_item_consume(IPluginContext *pContext, const cell_t *params);
        bool ItemGive(int id, const std::string &code, unsigned amount);
        cell_t x_item_give(IPluginContext *pContext, const cell_t *params);
        int ItemGet(int id, const std::string &code, bool use_cache = true);
        cell_t x_item_get(IPluginContext *pContext, const cell_t *params);

        void async_ItemConsume(int id, const std::string& code, unsigned amount, std::function<void(bool)> fn);
        cell_t x_item_consume_async(IPluginContext* pContext, const cell_t* params);
        void async_ItemGive(int id, const std::string& code, unsigned amount, std::function<void(bool)> fn);
        cell_t x_item_give_async(IPluginContext* pContext, const cell_t* params);
        
    }

}