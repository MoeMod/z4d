#pragma once

#include "sm/cstrike.h"
#include "sm/sourcemod.h"
#include "sm/sdktools.h"

#include <iostream>
#include <vector>
#include <algorithm>
#include <ranges>

namespace gameplay {
    inline namespace tools {

        inline int GetTeam(CBaseEntity *entity) {
            return sm::GetEntProp<int>(entity, sm::Prop_Data, "m_iTeamNum");
        }

        inline void SetTeam(CBaseEntity *entity, CSTeam_e iValue)
        {
            // Validate team
            if (iValue <= CS_TEAM_SPECTATOR) /// Fix, thanks to inklesspen!
            {
                // Sets team of the entity
                sm::sourcemod::ChangeClientTeam(playerhelpers->GetGamePlayer(gamehelpers->EntityToReference(entity)), iValue);
            }
            else
            {
                // Switch team of the entity
                sm::cstrike::CS_SwitchTeam(sm::ent_cast<CBasePlayer*>(entity), iValue);
            }
        }

        inline int GetFrags(CBaseEntity *entity)
        {
            return sm::GetEntProp<int>(entity, sm::Prop_Data, "m_iFrags");
        }

        inline void SetFrags(CBaseEntity *entity, int value)
        {
            sm::SetEntProp<int>(entity, sm::Prop_Data, "m_iFrags", value);
        }

        inline int GetDeaths(CBaseEntity *entity)
        {
            return sm::GetEntProp<int>(entity, sm::Prop_Data, "m_iDeaths");
        }

        inline void SetDeaths(CBaseEntity *entity, int value)
        {
            sm::SetEntProp<int>(entity, sm::Prop_Data, "m_iDeaths", value);
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

        inline bool GetHelmet(CBaseEntity *entity)
        {
            return sm::GetEntProp<bool>(entity, sm::Prop_Send, "m_bHasHelmet");
        }

        inline void SetHelmet(CBaseEntity *entity, bool bEnable)
        {
            sm::SetEntProp<bool>(entity, sm::Prop_Send, "m_bHasHelmet", bEnable);
        }

        inline bool GetHeavySuit(CBaseEntity *entity)
        {
            return sm::GetEntProp<bool>(entity, sm::Prop_Send, "m_bHasHeavyArmor");
        }

        inline void SetHeavySuit(CBaseEntity *entity, bool bEnable)
        {
            sm::SetEntProp<bool>(entity, sm::Prop_Send, "m_bHasHeavyArmor", bEnable);
        }

        inline CBaseCombatWeapon *GetActiveWeapon(CBasePlayer *entity)
        {
            return static_cast<CBaseCombatWeapon *>(sm::ent_cast<CBaseEntity *>(sm::GetEntProp<CBaseHandle>(entity, sm::Prop_Send, "m_hActiveWeapon")));
        }

        // TESTED 2020/3/25
        // TODO : change with std::ranges::views
        inline auto GetMyWeapons(CBasePlayer *entity)
        {
            return sm::GetEntPropArray<CBaseHandle>(entity, sm::Prop_Data, "m_hMyWeapons")
                | std::ranges::views::transform(sm::Converter<CBaseCombatWeapon*>())
                | std::ranges::views::filter(std::identity());
        }

        // TESTED 2020/3/25
        inline void RemoveAllWeapons(CBasePlayer *entity)
        {
            for(CBaseCombatWeapon *weapon : GetMyWeapons(entity))
            {
                sm::sdktools::RemovePlayerItem(entity, weapon);
                sm::sdktools::AcceptEntityInput(weapon, "Kill");
            }
        }
    }
}