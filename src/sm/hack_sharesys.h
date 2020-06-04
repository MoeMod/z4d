#pragma once

#include "smsdk_ext.h"
#include <core/logic/ShareSys.h>

inline SPVM_NATIVE_FUNC FindGlobalNative(const char* name)
{
	ShareSystem* derived = dynamic_cast<ShareSystem*>(sharesys);
	return derived->FindNative(name)->func();
}