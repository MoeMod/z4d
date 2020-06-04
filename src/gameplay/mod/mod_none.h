#pragma once

#include "mod.h"

namespace gameplay {
	namespace mod {

		class Mod_None : virtual public IBaseMod {
		public:
			void Event_OnRoundStart(IGameEvent* pEvent) override {}
			void Event_OnPlayerSpawn(IGameEvent* pEvent) override {}
			void Event_OnPlayerDeath(IGameEvent* pEvent) override {}

		};
	}
}

