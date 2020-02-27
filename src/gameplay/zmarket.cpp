#include "extension.h"

#include "zmarket.h"
#include "weapon_list.h"

#include "sm/sdktools.h"

#include <array>
#include <memory>
#include <string>
#include <numeric>
#include <vector>
#include <functional>

namespace gameplay {
    namespace zmarket {

        struct ClientData
        {
            ClientData() : unlocked(MAX_WEAPON_BUY_INFO, nullptr)
            {
                std::iota(unlocked.begin(), unlocked.end(), c_WeaponBuyList);
            }
            ClientData(const ClientData &) = delete;

            std::shared_ptr<IBaseMenu> menu;
            bool m_bAlreadyBoughtWeapon = false;

            std::vector<const WeaponBuyInfo *> unlocked; // sort it!

            const WeaponBuyInfo * m_pSelected[4] = {};
        };
        std::array<ClientData, SM_MAXPLAYERS + 1> g_ClientData;

        // void onSelected(IBaseMenu *menu, int client, unsigned int item)
        template<class Fn> std::shared_ptr<IBaseMenu> MakeMenu(Fn &&onSelected)
        {
            class MenuHandler : public IMenuHandler {
            public:
                explicit MenuHandler(Fn fn) : m_fnOnSelect(std::move(fn)) {}
                void OnMenuSelect(IBaseMenu *menu, int client, unsigned int item) override
                {
                    return m_fnOnSelect(menu, client, item);
                }
                Fn m_fnOnSelect;
            };
            auto handler = std::make_shared<MenuHandler>(std::forward<Fn>(onSelected));
            return std::shared_ptr<IBaseMenu>(menus->GetDefaultStyle()->CreateMenu(handler.get()), [handler](IBaseMenu *p) { p->Destroy(); });
        }

        void BuyAllWeapon(int id)
        {
            auto &cd = g_ClientData[id];
            CBaseEntity *player = gamehelpers->ReferenceToEntity(id);
            for(const WeaponBuyInfo *item : cd.m_pSelected)
            {
                if(item)
                    sm::sdktools::GivePlayerItem(player, item->entity);
            }
        }

        void ShowBuyMenu(int id)
        {
            auto &cd = g_ClientData[id];

            auto menu = MakeMenu([](IBaseMenu *menu, int id, int item) {
                if(item >= CS_SLOT_PRIMARY && item <= CS_SLOT_GRENADE)
                    ShowBuySubMenu(id, static_cast<CSWeaponSlot_e>(item));

                if(item == 4)
                    BuyAllWeapon(id);
            });
            menu->SetDefaultTitle("Buy Weapon");

            const char *SlotNames[4] = { "Primary", "Secondary", "Knife", "Grenade" };
            for(int i = 0; i < 4; ++i)
                menu->AppendItem(SlotNames[i], ItemDrawInfo((std::string(SlotNames[i]) + ":" + (cd.m_pSelected[i] ? (cd.m_pSelected[i]->name) : "not selected")).c_str()));

            if (!cd.m_bAlreadyBoughtWeapon)
                menu->AppendItem("4", ItemDrawInfo("Buy weapon now !"));

            menu->SetMenuOptionFlags(menu->GetMenuOptionFlags() | MENUFLAG_BUTTON_EXIT);
            menu->Display(id, MENU_TIME_FOREVER);

            // xu ming
            cd.menu = std::move(menu);
        }

        int GetUserMoney(int id)
        {
            // TODO
            return 0;
        }

        void SetUserMoney(int id, int delta)
        {
            // TODO
        }

        bool HasUnlockedWeapon(int id, const WeaponBuyInfo *item)
        {
            const auto &cd = g_ClientData[id];
            return std::binary_search(cd.unlocked.begin(), cd.unlocked.end(), item);
        }

        void OnSelectWeapon(int id, const WeaponBuyInfo *item)
        {
            auto &cd = g_ClientData[id];

            if(GetUserMoney(id) < item->cost)
                return;

            if(!HasUnlockedWeapon(id, item)) {
                SetUserMoney(id, -item->cost);
                cd.unlocked.push_back(item);
                // resort it
                std::inplace_merge(cd.unlocked.begin(), cd.unlocked.end() - 1, cd.unlocked.end());
            }

            cd.m_pSelected[item->slot] = item;
        }

        void ShowBuySubMenu(int id, CSWeaponSlot_e slot)
        {
            auto &cd = g_ClientData[id];

            // make all weapons list
            std::vector<const WeaponBuyInfo *> CurrentItems(MAX_WEAPON_BUY_INFO, nullptr);
            std::iota(CurrentItems.begin(), CurrentItems.end(), c_WeaponBuyList);
            // display current slot only
            CurrentItems.erase(std::remove_if(CurrentItems.begin(), CurrentItems.end(), [slot](const WeaponBuyInfo *item) { return item->slot == slot; }), CurrentItems.end());
            // unlocked weapon first
            std::partition(CurrentItems.begin(), CurrentItems.end(), [id](const WeaponBuyInfo *item){return HasUnlockedWeapon(id, item);});
            // no item, no menu
            if(CurrentItems.empty())
                return;

            auto menu = MakeMenu([CurrentItems, slot](IBaseMenu *menu, int id, int item) {
                OnSelectWeapon(id, CurrentItems[item]);
                // redraw menu
                ShowBuySubMenu(id, slot);
            });

            menu->RemoveAllItems();
            for(const WeaponBuyInfo *item : CurrentItems)
            {
                std::string display = item->name;
                unsigned int style = ITEMDRAW_DEFAULT;
                if(!HasUnlockedWeapon(id, item))
                {
                    display = std::move(display) + "\t" + std::to_string(item->cost) + "$";
                    if(GetUserMoney(id) < item->cost)
                        style = ITEMDRAW_DISABLED;
                }
                menu->AppendItem(item->entity, ItemDrawInfo(display.c_str(), style));
            }

            // xu ming
            cd.menu = std::move(menu);
        }

        class ClientListener : public IClientListener
        {
        public:
            void OnClientConnected(int client) override {
                // clear buy data
                g_ClientData[client] = {};
            }
        } g_ClientListener;

        void Init() {
            playerhelpers->AddClientListener(&g_ClientListener);
        }

    }
}