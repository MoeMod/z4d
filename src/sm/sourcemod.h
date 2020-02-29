#pragma once

#include <iserverunknown.h>
#include <iservernetworkable.h>
#include <server_class.h>
#include <stdexcept>

#include "hook_types.h"

namespace SourceMod {
    class IGamePlayer;
}

namespace sm {
    inline namespace sourcemod {

        constexpr struct {} Prop_Data = {};
        constexpr struct {} Prop_Send = {};

        template<class T = cell_t>
        T &EntProp(CBaseEntity *pEntity, decltype(Prop_Data), const char *prop, int size=sizeof(T), int element=0) {
            assert(pEntity != nullptr);
            sm_datatable_info_t info = {};
            if(!gamehelpers->FindDataMapInfo(gamehelpers->GetDataMap(pEntity), prop, &info))
                throw std::runtime_error("Property not found");
            typedescription_t *td = info.prop;
            ptrdiff_t offset = info.actual_offset + (element * (td->fieldSizeInBytes / td->fieldSize));

            T *data = (T *)((intptr_t)pEntity + offset);
            return *data;
        }
        template<class T = cell_t>
        const T &GetEntProp(CBaseEntity *pEntity, decltype(Prop_Data), const char *prop, int size=sizeof(T), int element=0) {
            return EntProp<T>(pEntity, Prop_Data, prop, size, element);
        }
        template<class T = cell_t>
        T &SetEntProp(CBaseEntity *pEntity, decltype(Prop_Data), const char *prop, const T &value, int size=sizeof(T), int element=0) {
            return EntProp<T>(pEntity, Prop_Data, prop, size, element) = value;
        }

        template<class T = cell_t>
        T &EntProp(CBaseEntity *pEntity, decltype(Prop_Send), const char *prop, int size = sizeof(T), int element=0) {
            assert(pEntity != nullptr);
            sm_sendprop_info_t info = {};
	        IServerNetworkable *pNet = ((IServerUnknown *)pEntity)->GetNetworkable();
	        if(!pNet)
	            throw std::runtime_error("Edict is not networkable");

            if(!gamehelpers->FindSendPropInfo(pNet->GetServerClass()->GetName(), prop, &info))
                throw std::runtime_error("Property not found");

            SendProp *pProp = info.prop;
            ptrdiff_t offset = info.actual_offset;

            T *data = (T *)((intptr_t)pEntity + offset);
            return *data;
        }
        template<class T = cell_t>
        const T &GetEntProp(CBaseEntity *pEntity, decltype(Prop_Send), const char *prop, int size=sizeof(T), int element=0) {
            return EntProp<T>(pEntity, Prop_Send, prop, size, element);
        }
        template<class T = cell_t>
        T &SetEntProp(CBaseEntity *pEntity, decltype(Prop_Send), const char *prop, const T &value, int size=sizeof(T), int element=0) {
            return EntProp<T>(pEntity, Prop_Send, prop, size, element) = value;
        }

        // core/logic/smn_players.cpp
        void ChangeClientTeam(IGamePlayer *pPlayer, int team);
        bool IsClientConnected(IGamePlayer *pPlayer);
        bool IsPlayerAlive(CBasePlayer *pPlayer);

        int GetHealth(IGamePlayer *pPlayer);
        int GetArmorValue(IGamePlayer *pPlayer);
    }

    inline CBaseEntity *CBaseEntityFrom(int client) {
        return gamehelpers->ReferenceToEntity(client);
    }

    inline IGamePlayer *IGamePlayerFrom(int client) {
        return playerhelpers->GetGamePlayer(client);
    }

};