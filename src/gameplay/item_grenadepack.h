//
// Created by 梅峰源 on 2020-05-04.
//

#ifndef Z4D_ITEM_GRENADEPACK_H
#define Z4D_ITEM_GRENADEPACK_H

#include "itemown.h"

struct HyUserOwnItemInfo;

namespace gameplay {
    namespace itemown {

        ItemStatus ItemSelectPre_GrenadePack(int id, const HyUserOwnItemInfo& ii);

        void ItemSelectPost_GrenadePack(int id, const HyUserOwnItemInfo& ii);

        namespace grenadepack {
            void OnClientInit(int id);
            void Event_NewRound();
        }
    }
}


#endif //Z4D_ITEM_GRENADEPACK_H
