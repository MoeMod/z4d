#pragma once

#include "mod.h"
#include "mod_none.h"
#include "alarm.h"

namespace sm::hack {
	struct TakeDamageInfo;
}

namespace gameplay {
	namespace mod {
		class Mod_ZP : private Mod_None, virtual public IBaseMod
		{
		public:
			void OnServerLoad() override;
			void Event_OnRoundStart(IGameEvent* pEvent) override;
			void Event_OnPlayerTeam(IGameEvent* pEvent) override;
			void OnClientPutInServer(int id) override;
			bool OnClientCommand(edict_t* pEntity, const CCommand& command) override;

		public:
			Mod_ZP();
			~Mod_ZP();

		protected:
			void OnTimer();
			void SelectZombieOrigin();
			bool MakeHuman(int id, int attacker = 0);
			bool MakeZombie(int id, int attacker = 0);
			bool ApplyOnClientTeam(int id, int old_team, int new_team);
			sm::Action OnAlarmShowPre(alarm::Alarm_s&);
			sm::Action OnTerminateRound(float, int);
			sm::HookResult<int> OnTakeDamage(CBaseEntity* entity, sm::hack::TakeDamageInfo&);

			void CheckWinConditions();
			void HumanWin();
			void ZombieWin();
			bool TimeExpired() const;

		private:
			enum GameStatus
			{
				GAMESTATUS_IDLE,
				GAMESTATUS_ROUNDSTARTED,
				GAMESTATUS_INFECTIONSTART,
				GAMESTATUS_ROUNDEND
			} m_iGameStatus = GAMESTATUS_IDLE;

			std::shared_ptr<ITimer> m_iTimerTask;
			int m_iTimerSecs = 0;
			EventListener m_eventAlarmShowPreListener;
			EventListener m_eventTerminateRoundListener;
			EventListener m_OnTakeDamageListener;
		};
	}
}