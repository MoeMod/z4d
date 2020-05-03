#pragma once

namespace gameplay {
    namespace itemown {

        void ShowItemOwnMenu(int id);
        void OnClientInit(int id);

        bool ItemConsume(int id, const std::string &code, unsigned amount);
        cell_t x_item_consume(IPluginContext *pContext, const cell_t *params);
        bool ItemGive(int id, const std::string &code, unsigned amount);
        cell_t x_item_give(IPluginContext *pContext, const cell_t *params);

        int ItemGet(int id, const std::string &code, bool use_cache = true);
        cell_t x_item_get(IPluginContext *pContext, const cell_t *params);
    }

}