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

        bool SDK_OnLoad(char* error, size_t maxlength, bool late);
        void SDK_OnUnload();

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
        bool IsPlayerAlive(IGamePlayer *pPlayer);
        bool IsPlayerAlive(CBaseEntity *pEntity);

        int GetHealth(IGamePlayer *pPlayer);
        int GetArmorValue(IGamePlayer *pPlayer);
    }

    inline namespace convert {
        inline CBaseEntity *edict2cbase(edict_t *edict) {
            assert(edict != nullptr);
            IServerUnknown *pUnknown = edict->GetUnknown();
            if (pUnknown == nullptr)
                return nullptr;
            CBaseEntity *pEntity = pUnknown->GetBaseEntity();
            return pEntity;
        }

        inline edict_t *id2edict(int id) {
            return gamehelpers->EdictOfIndex(id);
        }

        inline int edict2id(edict_t *edict) {
            return gamehelpers->IndexOfEdict(edict);
        }

        inline CBaseEntity *id2cbase(int id) {
            return gamehelpers->ReferenceToEntity(gamehelpers->IndexToReference(id));
        }

        inline int cbase2id(CBaseEntity *pEntity) {
            return gamehelpers->ReferenceToIndex(gamehelpers->EntityToReference(pEntity));
        }
    }

    inline CBaseEntity *CBaseEntityFrom(int client) {
        return id2cbase(client);
    }

    inline IGamePlayer *IGamePlayerFrom(int client) {
        return playerhelpers->GetGamePlayer(client);
    }


};