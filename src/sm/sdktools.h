#pragma once

using CBasePlayer = CBaseEntity;
class CEconItemView;

namespace sm {
    namespace sdktools{
        CBaseEntity *GivePlayerItem(CBasePlayer *player, const char *item, int iSubType = 0);

        bool SDK_OnLoad(char* error, size_t maxlength, bool late);
        void SDK_OnUnload();
    }
}