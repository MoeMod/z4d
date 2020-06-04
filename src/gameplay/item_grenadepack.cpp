//
// Created by 梅峰源 on 2020-05-04.
//

#include "extension.h"
#include "sm/sourcemod.h"
#include "sm/sdktools.h"
#include "util/smhelper.h"
#include "item_grenadepack.h"
#include "HyDatabase.h"
#include "weapon_list.h"

#include <bitset>
#include <vector>
#include <array>
#include <random>
#include <memory>
#include <functional>

namespace gameplay {
    namespace itemown {

        std::bitset<SM_MAXPLAYERS+1> g_bitsHasUsedGrenadePack;

        ItemStatus ItemSelectPre_GrenadePack(int id, const HyUserOwnItemInfo& ii)
        {
            if(ii.item.code == "tz_grenadepack")
            {
                return !g_bitsHasUsedGrenadePack.test(id) ? ItemStatus::Available : ItemStatus::Disabled;
            }
            return ItemStatus::Disabled;
        }

        void ItemSelectPost_GrenadePack(int id, const HyUserOwnItemInfo& ii)
        {
            if(ii.item.code == "tz_grenadepack" && ItemConsume(id, "tz_grenadepack", 1))
            {
                g_bitsHasUsedGrenadePack.set(id, true);
                sm::PrintToChat(id, (std::string() + " \x05[死神CS社区]\x01 您已经成功使用手雷包，将在每回合开局获得一颗随机手雷。").c_str());
            }
        }

        namespace grenadepack {

            void GivePlayerRandomGrenade(int id)
            {
                if(!sm::IsPlayerAlive(playerhelpers->GetGamePlayer(id)))
                    return;

                std::vector<WeaponBuyInfo> grenade_list;
                std::remove_copy_if(std::begin(c_WeaponBuyList), std::end(c_WeaponBuyList), std::back_inserter(grenade_list), [](const WeaponBuyInfo &_1){ return _1.slot != CS_SLOT_GRENADE; });

                WeaponBuyInfo item;
                std::sample(grenade_list.begin(), grenade_list.end(), &item, 1, std::mt19937(std::random_device()()));

                sm::sdktools::GivePlayerItem(sm::CBaseEntityFrom(id), item.entity);
                sm::PrintToChat(id, (std::string() + " \x05[死神CS社区]\x01 感谢您的支持，现在赠送死神手雷包 \x02" + item.name + " \x01 一颗。").c_str());
            }

            void OnClientInit(int id)
            {
                g_bitsHasUsedGrenadePack.set(id, false);
            }

            void Event_NewRound()
            {
                for(int id = 1; id <= playerhelpers->GetMaxClients(); ++id)
                {
                    if(!playerhelpers->GetGamePlayer(id)->IsConnected())
                        continue;

                    if (g_bitsHasUsedGrenadePack.test(id))
                    {
                        sm::CreateTimer(1.0, std::bind(GivePlayerRandomGrenade, id));
                    }
                }
            }
        }
    }
}