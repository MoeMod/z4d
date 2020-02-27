#include "extension.h"

#include "sm/sdkhooks.h"
#include "sm/sdktools.h"
#include "sm/cstrike.h"

#include "event.h"
#include "client.h"

#include "gameplay/gameplay.h"

/**
 * @file extension.cpp
 * @brief Implement extension code here.
 */

Sample g_Sample;		/**< Global singleton for extension's main interface */

SMEXT_LINK(&g_Sample);

bool Sample::SDK_OnLoad(char* error, size_t maxlen, bool late)
{
    sm::sdkhooks::SDK_OnLoad(error, maxlen, late);
    sm::sdktools::SDK_OnLoad(error, maxlen, late);
    sm::cstrike::SDK_OnLoad(error, maxlen, late);
    event::SDK_OnLoad(error, maxlen, late);
    client::SDK_OnLoad(error, maxlen, late);
    gameplay::SDK_OnLoad(error, maxlen, late);

    return SDKExtension::SDK_OnLoad(error, maxlen, late);
}

void Sample::SDK_OnAllLoaded() {
    return SDKExtension::SDK_OnAllLoaded();
}

void Sample::SDK_OnUnload() {
    sm::sdkhooks::SDK_OnUnload();
    sm::sdktools::SDK_OnUnload();
    sm::cstrike::SDK_OnUnload();
    event::SDK_OnUnload();
    client::SDK_OnUnload();
    gameplay::SDK_OnUnload();

    return SDKExtension::SDK_OnUnload();
}

bool Sample::SDK_OnMetamodLoad(ISmmAPI *ismm, char *error, size_t maxlen, bool late) {
    event::SDK_OnMetamodLoad(ismm, error, maxlen, late);
    client::SDK_OnMetamodLoad(ismm, error, maxlen, late);

    return SDKExtension::SDK_OnMetamodLoad(ismm, error, maxlen, late);
}
