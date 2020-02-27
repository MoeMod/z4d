#pragma once

namespace event {
    bool Event_SDK_OnLoad(char *error, size_t maxlength, bool late);
    void Event_SDK_OnAllLoaded();
    bool Event_SDK_OnMetamodLoad(ISmmAPI *ismm, char *error, size_t maxlen, bool late);
}