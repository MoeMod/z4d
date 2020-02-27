#pragma once

#include <stdlib.h>

#include <isaverestore.h>

#ifndef _DEBUG
#include <shared/ehandle.h>
#else
#undef _DEBUG
#include <shared/ehandle.h>
#define _DEBUG 1
#endif

#include <server_class.h>

#include "vector.h"

#include <shared/shareddefs.h>
#include <shared/takedamageinfo.h>

namespace sm {
    struct TakeDamageInfo : ::CTakeDamageInfo
    {
        // make everything public :)
        using ::CTakeDamageInfo::m_vecDamageForce;
        using ::CTakeDamageInfo::m_vecDamagePosition;
        using ::CTakeDamageInfo::m_vecReportedPosition;	// Position players are told damage is coming from
        using ::CTakeDamageInfo::m_hInflictor;
        using ::CTakeDamageInfo::m_hAttacker;
        using ::CTakeDamageInfo::m_hWeapon;
        using ::CTakeDamageInfo::m_flDamage;
        using ::CTakeDamageInfo::m_flMaxDamage;
        using ::CTakeDamageInfo::m_flBaseDamage;			// The damage amount before skill leve adjustments are made. Used to get uniform damage forces.
        using ::CTakeDamageInfo::m_bitsDamageType;
        using ::CTakeDamageInfo::m_iDamageCustom;
        using ::CTakeDamageInfo::m_iDamageStats;
        using ::CTakeDamageInfo::m_iAmmoType;			// AmmoType of the weapon used to cause this damage, if any
        using ::CTakeDamageInfo::m_flRadius;

        // CS:GO
        using ::CTakeDamageInfo::m_iDamagedOtherPlayers;
        using ::CTakeDamageInfo::m_iObjectsPenetrated;
        using ::CTakeDamageInfo::m_uiBulletID;
        using ::CTakeDamageInfo::m_uiRecoilIndex;
    };
}