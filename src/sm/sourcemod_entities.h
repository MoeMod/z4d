#pragma once

namespace sm {
	inline namespace entities {
		inline const char* GetEdictClassname(edict_t* ed)
		{
			return gamehelpers->GetEntityClassname(ed);
		}
	}
}