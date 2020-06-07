#pragma once

class IGameEventManager2;

namespace event {
    extern IGameEventManager2* gameevents;
    bool SDK_OnLoad(char *error, size_t maxlength, bool late);
    bool SDK_OnMetamodLoad(ISmmAPI *ismm, char *error, size_t maxlen, bool late);
    void SDK_OnUnload();
}