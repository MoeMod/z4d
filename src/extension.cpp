#include "extension.h"

#include "hook/hook_mp.h"
#include "hook/hook_cs.h"

#include "event.h"

/**
 * @file extension.cpp
 * @brief Implement extension code here.
 */

Sample g_Sample;		/**< Global singleton for extension's main interface */

SMEXT_LINK(&g_Sample);

bool Sample::SDK_OnLoad(char* error, size_t maxlen, bool late)
{
    hook::hooks().SDK_OnLoad(error, maxlen, late);
    hook::CS_SDK_OnLoad(error, maxlen, late);
    event::Event_SDK_OnLoad(error, maxlen, late);

    return SDKExtension::SDK_OnLoad(error, maxlen, late);
}

void Sample::SDK_OnAllLoaded() {
    hook::hooks().SDK_OnAllLoaded();
    hook::CS_SDK_OnAllLoaded();
    event::Event_SDK_OnAllLoaded();

    return SDKExtension::SDK_OnAllLoaded();
}

void Sample::SDK_OnUnload() {
    hook::hooks().SDK_OnUnload();
    hook::CS_SDK_OnUnload();

    return SDKExtension::SDK_OnUnload();
}

bool Sample::SDK_OnMetamodLoad(ISmmAPI *ismm, char *error, size_t maxlen, bool late) {
    event::Event_SDK_OnMetamodLoad(ismm, error, maxlen, late);

    return SDKExtension::SDK_OnMetamodLoad(ismm, error, maxlen, late);
}
