#pragma once

#include "hook_takedamageinfo.h"

class CBaseCombatWeapon;

namespace hook {

    class hooks {

    protected:
        void SetupHooks();

    };

    namespace call {
        void Takedamage(CBaseEntity *pEntity, const TakeDamageInfo &);
        void DropWeapon(CBaseEntity *pPlayer, CBaseCombatWeapon *pWeapon, const Vector *pvecTarget, const Vector *pVelocity);
    }


}