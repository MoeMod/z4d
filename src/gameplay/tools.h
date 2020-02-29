#pragma once

#include "sm/cstrike.h"
#include "sm/sourcemod.h"

namespace gameplay {
    inline namespace tools {

        inline int GetTeam(CBaseEntity *entity) {
            return sm::GetEntProp<int>(entity, sm::Prop_Data, "m_iTeamNum");
        }

        inline void SetTeam(CBaseEntity *entity, CSTeam_e iValue)
        {
            // Validate team
            if (GetTeam(entity) <= CS_TEAM_SPECTATOR) /// Fix, thanks to inklesspen!
            {
                // Sets team of the entity
                sm::sourcemod::ChangeClientTeam(playerhelpers->GetGamePlayer(gamehelpers->EntityToReference(entity)), iValue);
            }
            else
            {
                // Switch team of the entity
                sm::cstrike::CS_SwitchTeam(entity, iValue);
            }
        }

        inline bool GetDefuser(CBaseEntity *entity)
        {
            return sm::GetEntProp<bool>(entity, sm::Prop_Send, "m_bHasDefuser");
        }

        inline void SetDefuser(CBaseEntity *entity, bool bEnable)
        {
            sm::SetEntProp<bool>(entity, sm::Prop_Send, "m_bHasDefuser", bEnable);
        }

        inline int GetHealth(CBaseEntity *entity, bool bMax = false)
        {
            return sm::GetEntProp<int>(entity, sm::Prop_Data, bMax ? "m_iMaxHealth" : "m_iHealth");
        }

        inline void SetHealth(CBaseEntity *entity, int iValue, bool bSet = false)
        {
            sm::SetEntProp<int>(entity, sm::Prop_Send, "m_iHealth", iValue);
            if(bSet)
                sm::SetEntProp<int>(entity, sm::Prop_Data, "m_iMaxHealth", iValue);
        }

        inline int GetArmor(CBaseEntity *entity)
        {
            return sm::GetEntProp<int>(entity, sm::Prop_Send, "m_ArmorValue");
        }

        inline void SetArmor(CBaseEntity *entity, int iValue)
        {
            sm::SetEntProp<int>(entity, sm::Prop_Send, "m_ArmorValue", iValue);
        }

        inline float GetMaxspeedMultiplier(CBaseEntity *entity)
        {
            return sm::GetEntProp<float>(entity, sm::Prop_Send, "m_flLaggedMovementValue");
        }

        inline void SetMaxspeedMultiplier(CBaseEntity *entity, float flValue)
        {
            sm::SetEntProp<float>(entity, sm::Prop_Send, "m_flLaggedMovementValue", flValue);
        }

        inline float GetGravity(CBaseEntity *entity)
        {
            return sm::GetEntProp<float>(entity, sm::Prop_Send, "m_flGravity");
        }

        inline void SetGravity(CBaseEntity *entity, float flValue)
        {
            sm::SetEntProp<float>(entity, sm::Prop_Send, "m_flGravity", flValue);
        }
    }
}