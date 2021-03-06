#pragma once

#include "sm/cstrike_const.h"

namespace gameplay {
    struct WeaponBuyInfo {
        const char *entity;
        const char *name;
        CSWeaponSlot_e slot;
        int cost = 0;
    };

    constexpr bool operator<(const WeaponBuyInfo &a, const WeaponBuyInfo &b)
    {
        return a.entity < b.entity;
    }

    constexpr WeaponBuyInfo c_WeaponBuyList[] = {
            // pistol
            { "weapon_elite", "Elites", CS_SLOT_SECONDARY, 0 },
            { "weapon_deagle", "Desert Eagle", CS_SLOT_SECONDARY, 0 },
            { "weapon_usp_silencer", "USP-S", CS_SLOT_SECONDARY, 0 },
            { "weapon_hkp2000", "HKP2000", CS_SLOT_SECONDARY, 0 },
            { "weapon_fiveseven", "FiveSeven", CS_SLOT_SECONDARY, 0 },
            { "weapon_tec9", "Tec-9", CS_SLOT_SECONDARY, 0 },
            { "weapon_cz75a", "CZ75A", CS_SLOT_SECONDARY, 0 },
            { "weapon_p250", "P250", CS_SLOT_SECONDARY, 0 },
            { "weapon_taser", "TASER", CS_SLOT_SECONDARY, 0 },
            { "weapon_revolver", "R8 Revolver", CS_SLOT_SECONDARY, 0 },
            // shotgun
            { "weapon_nova", "Nova", CS_SLOT_PRIMARY, 0 },
            { "weapon_sawedoff", "Sawed-Off", CS_SLOT_PRIMARY, 0 },
            { "weapon_xm1014", "XM1014", CS_SLOT_PRIMARY, 0 },
            { "weapon_mag7", "Mag-7", CS_SLOT_PRIMARY, 0 },
            // rifle
            { "weapon_ak47", "CV-47", CS_SLOT_PRIMARY, 0 },
            { "weapon_sg556", "SG556", CS_SLOT_PRIMARY, 0 },
            { "weapon_aug", "AUG", CS_SLOT_PRIMARY, 0 },
            { "weapon_m4a1", "M4A4", CS_SLOT_PRIMARY, 0 },
            { "weapon_m4a1_silencer", "M4A1-S", CS_SLOT_PRIMARY, 0 },
            { "weapon_bizon", "Bizon", CS_SLOT_PRIMARY, 0 },
            { "weapon_famas", "Famas", CS_SLOT_PRIMARY, 0 },
            { "weapon_galilar", "Galil AR", CS_SLOT_PRIMARY, 0 },
            // smg
            { "weapon_mac10", "Mac-10", CS_SLOT_PRIMARY, 0 },
            { "weapon_mp7", "MP7", CS_SLOT_PRIMARY, 0 },
            { "weapon_mp5sd", "MP5-SD", CS_SLOT_PRIMARY, 0 },
            { "weapon_mp9", "MP9", CS_SLOT_PRIMARY, 0 },
            { "weapon_p90", "ES C90", CS_SLOT_PRIMARY, 0 },
            { "weapon_ump45", "UWP45", CS_SLOT_PRIMARY, 0 },
            // sniper
            { "weapon_awp", "AWP Magnum", CS_SLOT_PRIMARY, 0 },
            { "weapon_ssg08", "SSG-08", CS_SLOT_PRIMARY, 0 },
            { "weapon_g3sg1", "G3-SG1", CS_SLOT_PRIMARY, 0 },
            { "weapon_scar20", "Scar-20", CS_SLOT_PRIMARY, 0 },
            // mg
            { "weapon_m249", "M249", CS_SLOT_PRIMARY, 0 },
            { "weapon_negev", "Negev", CS_SLOT_PRIMARY, 0 },
            // knife
            { "weapon_knife", "Knife", CS_SLOT_KNIFE, 0 },
            { "weapon_fists", "Fists", CS_SLOT_KNIFE, 0 },
            { "weapon_knifegg", "Knife-GG", CS_SLOT_KNIFE, 0 },
            { "weapon_knife_t", "Knife-T", CS_SLOT_KNIFE, 0 },
            // grenade
            { "weapon_tagrenade", "战术侦测手雷 / TA Grenade", CS_SLOT_GRENADE, 0 },
            { "weapon_hegrenade", "高爆手雷 / HE Grenade", CS_SLOT_GRENADE, 0 },
            { "weapon_smokegrenade", "烟雾弹 / SG Grenade", CS_SLOT_GRENADE, 0 },
            { "weapon_flashbang", "闪光弹 / FB Grenade", CS_SLOT_GRENADE, 0 },
            { "weapon_decoy", "诱饵弹 / Decoy", CS_SLOT_GRENADE, 0 },
            { "weapon_incgrenade", "燃烧弹 / INC Grenade", CS_SLOT_GRENADE, 0 },
            { "weapon_molotov", "燃烧瓶 / Molotov", CS_SLOT_GRENADE, 0 },
            { "weapon_snowball", "雪球 / Snow Ball", CS_SLOT_GRENADE, 0 },
            // others
            { "weapon_c4", "C4", CS_SLOT_C4, 0 },
            { "weapon_tablet", "平板电脑 / Tablet", CS_SLOT_UTILITY, 0 },
            { "weapon_healthshot", "注射器 / Healthshot", CS_SLOT_BOOST, 0 }
    };

    constexpr auto MAX_WEAPON_BUY_INFO = std::extent<decltype(c_WeaponBuyList)>::value;
}