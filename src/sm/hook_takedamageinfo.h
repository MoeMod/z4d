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

        inline int GetAttacker() const { return m_hAttacker.IsValid() ? m_hAttacker.GetEntryIndex() : -1; }
        inline int GetInflictor() const { return m_hInflictor.IsValid() ? m_hInflictor.GetEntryIndex() : -1; }
#if SOURCE_ENGINE >= SE_ORANGEBOX && SOURCE_ENGINE != SE_LEFT4DEAD
        inline int GetWeapon() const { return m_hWeapon.IsValid() ? m_hWeapon.GetEntryIndex() : -1; }
#else
        inline int GetWeapon() const { return -1; }
        inline void SetWeapon(CBaseEntity*) {}
#endif

        inline void SetDamageForce(vec_t x, vec_t y, vec_t z)
        {
            m_vecDamageForce.x = x;
            m_vecDamageForce.y = y;
            m_vecDamageForce.z = z;
        }

        inline void SetDamagePosition(vec_t x, vec_t y, vec_t z)
        {
            m_vecDamagePosition.x = x;
            m_vecDamagePosition.y = y;
            m_vecDamagePosition.z = z;
        }
#if SOURCE_ENGINE < SE_ORANGEBOX
        inline int GetDamageCustom() const { return GetCustomKill(); }
#endif
    };
}