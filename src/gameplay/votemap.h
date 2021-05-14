#pragma once
#include "itemown.h"

struct HyUserOwnItemInfo;

namespace gameplay {
    namespace itemown {
        ItemStatus ItemSelectPre_VoteMap(int id, const HyUserOwnItemInfo& ii);
        void ItemSelectPost_VoteMap(int id, const HyUserOwnItemInfo& ii);
    }
    namespace votemap {
        void Init();
        bool Show_StartVoteMenu(int id);
    }
}