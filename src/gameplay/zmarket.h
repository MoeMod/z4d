#pragma once

enum CSWeaponSlot_e : int;

namespace gameplay {
    namespace zmarket {

        void Init();
        void ShowBuyMenu(int id);
        void ShowBuySubMenu(int id, CSWeaponSlot_e slot);
    }

}
