#pragma once
#include "itemown.h"

struct HyUserOwnItemInfo;

namespace gameplay {
    namespace itemown {
        ItemStatus ItemSelectPre_VoteKick(int id, const HyUserOwnItemInfo& ii);
        void ItemSelectPost_VoteKick(int id, const HyUserOwnItemInfo& ii);
    }
    namespace votekick {
        void Init();
        bool Show_StartVoteMenu(int id);
    }
}