#include "extension.h"

#include "sm/sourcemod.h"

#include "event.h"
#include "client.h"
#include "entity_listener.h"

#include "gameplay/gameplay.h"

/**
 * @file extension.cpp
 * @brief Implement extension code here.
 */

Sample g_Sample;		/**< Global singleton for extension's main interface */

SMEXT_LINK(&g_Sample);

bool Sample::SDK_OnLoad(char* error, size_t maxlen, bool late)
{
    sm::SDK_OnLoad(error, maxlen, late);
    event::SDK_OnLoad(error, maxlen, late);
    client::SDK_OnLoad(error, maxlen, late);
    entity_listener::SDK_OnLoad(error, maxlen, late);
    gameplay::SDK_OnLoad(error, maxlen, late);

    return SDKExtension::SDK_OnLoad(error, maxlen, late);
}

void Sample::SDK_OnAllLoaded() {
    return SDKExtension::SDK_OnAllLoaded();
}

void Sample::SDK_OnUnload() {
    // reversed calling sequence !
    gameplay::SDK_OnUnload();
    entity_listener::SDK_OnUnload();
    client::SDK_OnUnload();
    event::SDK_OnUnload();
    sm::SDK_OnUnload();

    return SDKExtension::SDK_OnUnload();
}

bool Sample::SDK_OnMetamodLoad(ISmmAPI *ismm, char *error, size_t maxlen, bool late) {
    event::SDK_OnMetamodLoad(ismm, error, maxlen, late);
    client::SDK_OnMetamodLoad(ismm, error, maxlen, late);
    entity_listener::SDK_OnMetamodLoad(ismm, error, maxlen, late);

    return SDKExtension::SDK_OnMetamodLoad(ismm, error, maxlen, late);
}
