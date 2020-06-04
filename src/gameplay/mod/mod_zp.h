#pragma once

#include "mod.h"
#include "mod_none.h"
#include "alarm.h"

namespace gameplay {
	namespace mod {
		class Mod_ZP : private Mod_None, virtual public IBaseMod
		{
		public:
			void Event_OnRoundStart(IGameEvent* pEvent);

		public:
			Mod_ZP();
			~Mod_ZP();

		protected:
			void OnTimer();
			void SelectZombieOrigin();
			void MakeHumans();
			sm::Action OnAlarmShowPre(alarm::Alarm_s&);

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
		};
	}
}