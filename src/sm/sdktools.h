#pragma once

#include "hook_types.h"

namespace sm {
    namespace sdktools{
        void RemovePlayerItem(CBasePlayer * player, CBaseCombatWeapon *pItem);
        CBaseEntity *GivePlayerItem(CBasePlayer *player, const char *item, int iSubType = 0);
        void SetLightStyle(int style, const char *value);
        int GivePlayerAmmo(CBasePlayer * player, int amount, int ammotype, bool suppressSound=false);
        void SetEntityModel(CBaseEntity *entity, const char *model);

        bool SDK_OnLoad(char* error, size_t maxlength, bool late);
        void SDK_OnUnload();
    }
}