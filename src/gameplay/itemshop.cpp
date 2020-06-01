#include "extension.h"
#include "itemshop.h"
#include "util/ImMenu.hpp"
#include "sm/sourcemod.h"
#include "itemown.h"

#include "util/ImMenu.hpp"
#include "HyDatabase.h"

namespace gameplay {
	namespace itemshop {

        void GetCachedItemShopListAsync(std::function<void(const std::vector<HyShopEntry>&)> callback)
        {
            static std::vector<HyShopEntry> cache;
            if (!cache.empty())
                return callback(cache);
            HyDatabase().async_QueryShopEntry([callback](std::vector<HyShopEntry> res) {
                sm::RunOnMainThread([callback, res = std::move(res)]() mutable {
                    cache = std::move(res);
                    return callback(cache);
                });
            });
        }

        void ApplyItemShopEntry(int id, const HyShopEntry& se)
        {
            itemown::async_ItemConsume(id, se.exchange_item.code, se.exchange_amount, [id, se](bool success) {
                if (success)
                {
                    sm::RunOnMainThread(itemown::async_ItemGive, id, se.target_item.code, se.target_amount, [id, se](bool success) {
                        if (success)
                        {
                            sm::RunOnMainThread(sm::PrintToChatStr, id, std::string() +
                                " \x05[死神CS社区]\x01 您已经成功购买\x02" + std::to_string(se.target_amount) + se.target_item.quantifier + se.target_item.name + "\x01。"
                                );
                            sm::RunOnMainThread(Show_ItemShopMenu, id);
                        }
                        else
                        {
                            // TODO : 已经扣币但是给玩家发道具失败的情况下，可能要记日志。
                            sm::RunOnMainThread(sm::PrintToChat, id, 
                                " \x05[死神CS社区]\x01 商店出现了错误，请联系服主或者技术进行处理。");
                            // 不发回去了，防止此处出现bug刷道具
                            //itemown::ItemGive(id, se.exchange_item.code, se.exchange_amount);
                        }
                    });
                }
                else
                {
                    sm::RunOnMainThread(sm::PrintToChatStr, id, std::string() +
                        " \x05[死神CS社区]\x01 购买本道具需要消耗\x02" + std::to_string(se.exchange_amount) + se.exchange_item.quantifier + se.exchange_item.name +
                        "\x01，而你只有\x02" + std::to_string(itemown::ItemGet(id, se.exchange_item.code, true)) + se.exchange_item.quantifier + "，去多参与活动获得更多道具吧。"
                        );
                }
            });
        }

        void Show_ItemShopAskAgainMenu(int id, const HyShopEntry& se)
        {
            util::ImMenu([id, se](auto context) {
                context.begin("道具商店 / ItemShop" 
                    "\n\t是否确认使用"
                    "\n\t" + std::to_string(se.exchange_amount) + se.exchange_item.quantifier + se.exchange_item.name +
                    "\n\t进行兑换"
                    "\n\t" + std::to_string(se.target_amount) + se.target_item.quantifier + se.target_item.name +
                    "\n"
                );
                if(context.item("yes", "我同意 / Yes"))
                    ApplyItemShopEntry(id, se);
                if(context.item("no", "返回 / No"))
                    Show_ItemShopMenu(id);
                context.end(id);
            });
        }

		void Show_ItemShopMenu(int id)
		{
            GetCachedItemShopListAsync([id](const std::vector<HyShopEntry>& vecse) {
                util::ImMenu([id, vecse](auto context) {
                    context.begin("道具商店 / Item Shop");

                    for (const auto &se : vecse)
                    {
                        if (itemown::ItemGet(id, se.exchange_item.code, true) < se.exchange_amount)
                            context.disabled();
                        if (context.item(std::to_string(se.shopid),
                            se.target_item.name + std::to_string(se.target_amount) + se.target_item.quantifier +
                            "\t价格: " + se.exchange_item.name + std::to_string(se.exchange_amount) + se.exchange_item.quantifier
                            )) 
                        {
                            Show_ItemShopAskAgainMenu(id, se);
                        }
                    }

                    context.end(id);
                });
            });
		}
	}
}