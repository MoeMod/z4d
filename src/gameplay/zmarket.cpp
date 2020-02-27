#include "extension.h"

#include "zmarket.h"

#include <array>
#include <memory>
#include <string>

struct ClientData
{
    std::shared_ptr<IBaseMenu> menu;
    std::shared_ptr<IMenuHandler> handler;
    bool m_bAlreadyBoughtWeapon = false;
};

std::array<ClientData, SM_MAXPLAYERS + 1> g_ClientData;

class MainBuyMenuHandler : public IMenuHandler {
public:
    void OnMenuSelect(IBaseMenu *menu, int client, unsigned int item) override
    {
        // TODO
        engine->ClientPrintf(playerhelpers->GetGamePlayer(client)->GetEdict(), ("You have chosen %d" + std::to_string(item)).c_str());
    }
};

namespace gameplay {
    namespace zmarket {
        void ShowBuyMenu(int id) {
            auto &cd = g_ClientData[id];
            std::shared_ptr<MainBuyMenuHandler> handler = std::make_shared<MainBuyMenuHandler>();
            std::shared_ptr<IBaseMenu> menu(menus->GetDefaultStyle()->CreateMenu(handler.get()), [](IBaseMenu *p) { p->Destroy(); });
            menu->SetDefaultTitle("Buy Weapon");

            menu->AppendItem("1", ItemDrawInfo("Primary : "));
            menu->AppendItem("2", ItemDrawInfo("Secondary : "));
            menu->AppendItem("3", ItemDrawInfo("Knife : "));
            menu->AppendItem("4", ItemDrawInfo("Grenade : "));

            if (!cd.m_bAlreadyBoughtWeapon)
                menu->AppendItem("5", ItemDrawInfo("Buy weapon now !"));

            menu->SetMenuOptionFlags(menu->GetMenuOptionFlags() | MENUFLAG_BUTTON_EXIT);
            menu->Display(id, MENU_TIME_FOREVER);

            cd.menu = menu;
            cd.handler = handler;
        }

    }
}