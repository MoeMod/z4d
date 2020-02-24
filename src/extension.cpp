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

    return true;
}
