#pragma once

#include <memory>

class IGameEvent;

namespace gameplay {
	namespace mod {
		class IBaseMod
		{
		public:
			virtual ~IBaseMod() = default;
			virtual void Event_OnRoundStart(IGameEvent* pEvent) = 0;
			virtual void Event_OnRoundEnd(IGameEvent* pEvent) = 0;
			virtual void Event_OnPlayerSpawn(IGameEvent* pEvent) = 0;
			virtual void Event_OnPlayerDeath(IGameEvent* pEvent) = 0;
			virtual void Event_OnPlayerTeam(IGameEvent* pEvent) = 0;
			virtual void OnClientPutInServer(int id) = 0;
			virtual bool OnClientCommand(edict_t* pEntity, const CCommand& command) = 0;
			virtual void OnServerLoad() = 0;
			virtual void OnMapStart() = 0;
		};

		extern std::shared_ptr<IBaseMod> g_pModRunning;

		void Init();
	}
}
