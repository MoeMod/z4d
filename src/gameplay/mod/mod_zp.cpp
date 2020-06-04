
#include "extension.h"
#include "sm/sourcemod.h"
#include "sm/cstrike.h"
#include "mod_zp.h"
#include "alarm.h"
#include "teammgr.h"
#include "zmarket.h"
#include "zombie.h"

namespace gameplay {
	namespace mod {

		Mod_ZP::Mod_ZP()
		{
			m_eventAlarmShowPreListener = alarm::g_fwAlarmShowPre.subscribe(&Mod_ZP::OnAlarmShowPre, this);
			// block roundend
			//m_eventTerminateRoundListener = sm::cstrike::CS_OnTerminateRound.subscribe(sm::Plugin_Stop);
		}

		Mod_ZP::~Mod_ZP()
		{

		}

		void Mod_ZP::OnTimer()
		{
			++m_iTimerSecs;

			if (m_iTimerSecs == 20 && m_iGameStatus == GAMESTATUS_ROUNDSTARTED)
			{
				SelectZombieOrigin();
				m_iGameStatus = GAMESTATUS_INFECTIONSTART;
			}

			m_iTimerTask = sm::CreateTimerRAII(1.0, std::bind(&Mod_ZP::OnTimer, this));
		}

		void Mod_ZP::SelectZombieOrigin()
		{
			int candidate[SM_MAXPLAYERS + 4];
			int* candidate_end = candidate;
			for (int id = 1; id <= playerhelpers->GetMaxClients(); ++id)
			{
				auto igp = sm::IGamePlayerFrom(id);
				if (!sm::IsClientConnected(igp))
					continue;
				if (sm::IsPlayerAlive(igp) && teammgr::Team_Get(id) == CS_TEAM_CT)
				{
					*candidate_end++ = id;
				}
			}
			auto iZombieAmount = std::distance(candidate, candidate_end) / 10 + 1;
			std::for_each(candidate, candidate_end, std::bind(zombie::Originate, std::placeholders::_1, iZombieAmount, false));

			alarm::AlarmPush({ alarm::ALARMTYPE_INFECT, Color{255,255,255}, 1.0f, "首例感染已出现" });
		}

		sm::Action Mod_ZP::OnAlarmShowPre(alarm::Alarm_s& a)
		{
			if (a.type == alarm::ALARMTYPE_IDLE)
			{
				if (m_iGameStatus == GAMESTATUS_IDLE)
				{
					a.title = "正在等待玩家进入\nWaiting";
				}
				else if(m_iGameStatus == GAMESTATUS_ROUNDSTARTED)
				{
					a.title = "首例感染将在 " + std::to_string(20 - m_iTimerSecs) + " 秒后出现";\
				}
				else if (m_iGameStatus == GAMESTATUS_INFECTIONSTART)
				{
					a.title = std::to_string(teammgr::TeamCount(teammgr::ZB_TEAM_HUMAN, true)) + " HUMAN\tVS\tZOMBIE " + std::to_string(teammgr::TeamCount(teammgr::ZB_TEAM_HUMAN, true));
				}
				return sm::Plugin_Continue;
			}

			return sm::Plugin_Continue;
		}

		void Mod_ZP::MakeHumans()
		{
			for (int id = 1; id <= playerhelpers->GetMaxClients(); ++id)
			{
				auto igp = sm::IGamePlayerFrom(id);
				if (!sm::IsClientConnected(igp))
					continue;
				teammgr::Team_Set(id, teammgr::ZB_TEAM_HUMAN);
				zmarket::ShowBuyMenu(id);
			}
		}

		void Mod_ZP::Event_OnRoundStart(IGameEvent* pEvent)
		{
			sm::RequestFrame(&Mod_ZP::MakeHumans, this);
			m_iGameStatus = GAMESTATUS_ROUNDSTARTED;

			m_iTimerTask = sm::CreateTimerRAII(1.0, std::bind(&Mod_ZP::OnTimer, this));
			m_iTimerSecs = 0;
		}
	}
}

