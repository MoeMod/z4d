#include "extension.h"

#include "hook/hook_mp.h"

/**
 * @file extension.cpp
 * @brief Implement extension code here.
 */

Sample g_Sample;		/**< Global singleton for extension's main interface */

SMEXT_LINK(&g_Sample);

bool Sample::SDK_OnLoad(char* error, size_t maxlen, bool late)
{
    hook::hooks().SDK_OnLoad(error, maxlen, late);

    return SDKExtension::SDK_OnLoad(error, maxlen, late);
}

void Sample::SDK_OnAllLoaded() {
    hook::hooks().SDK_OnAllLoaded();
    return SDKExtension::SDK_OnAllLoaded();
}

void Sample::SDK_OnUnload() {
    hook::hooks().SDK_OnUnload();
    return SDKExtension::SDK_OnUnload();
}
