#pragma once

#include "util/strutil.h"
#include <string>
#include <stdarg.h>

namespace sm {
	inline namespace console {
		inline void FakePlayerCommand(edict_t* client, const char* format, ...)
		{
			va_list va;
			va_start(va, format);
			char buffer[256];
			vsnprintf(buffer, 255, format, va);
			serverpluginhelpers->ClientCommand(client, buffer);
		}
	}
}
