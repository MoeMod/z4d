#pragma once

namespace sm{
    inline namespace sourcemod {
        inline namespace players {

            inline const char *GetClientName(int id) {
                if(!id)
                    return icvar->FindVar("hostname")->GetString();
                return playerhelpers->GetGamePlayer(id)->GetName();
            }

        }
    }
}