
#include "extension.h"
#include "sm/sourcemod.h"
#include "sm/cstrike.h"
#include "sm/sdkhooks.h"
#include "mod_zp.h"
#include "alarm.h"
#include "teammgr.h"
#include "zmarket.h"
#include "zombie.h"
#include "admin.h"
#include "command.h"
#include <IGameEvents.h>
#include <random>

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

		void Mod_ZP::OnServerLoad()
		{
			if (ConVar* cv = command::icvar->FindVar("bot_quota_mode"))
				cv->SetValue("normal");
			if (ConVar* cv = command::icvar->FindVar("mp_autoteambalance"))
				cv->SetValue("0");
			if (ConVar* cv = command::icvar->FindVar("mp_limitteams"))
				cv->SetValue("0");
			if (ConVar* cv = command::icvar->FindVar("mp_ignore_round_win_conditions"))
				cv->SetValue("1");
			if (ConVar* cv = command::icvar->FindVar("bot_auto_vacate"))
				cv->SetValue("0");
		}

		void Mod_ZP::OnClientPutInServer(int id)
		{
			if(!m_OnPlayerSpawnPostListener)
				m_OnPlayerSpawnPostListener = sm::sdkhooks::SDKHookRAII(sm::id2cbase(id), sm::sdkhooks::SDKHook_SpawnPost, 
					std::bind(&Mod_ZP::OnPlayerSpawnPost, this, std::placeholders::_1));

			if (m_iGameStatus == GAMESTATUS_IDLE && teammgr::TeamCount(teammgr::ZB_TEAM_ZOMBIE) + teammgr::TeamCount(teammgr::ZB_TEAM_HUMAN) >= 2)
			{
				m_iGameStatus = GAMESTATUS_ROUNDSTARTED;
				admin::RestartGame();
			}
		}

		bool Mod_ZP::OnClientCommand(edict_t* pEntity, const CCommand& command)
		{
			int id = sm::edict2id(pEntity);
			if (id && !strcmp(command.Arg(0), "jointeam"))
			{
				char* end;
				if (const int new_team = std::strtol(command.Arg(1), &end, 10); end)
				{
					const auto old_team = teammgr::Team_Get(id);
					if (ApplyOnClientTeam(id, old_team, new_team))
						return true;
				}
			}
			return false;
		}

		void Mod_ZP::Event_OnPlayerTeam(IGameEvent* pEvent)
		{
			int client = playerhelpers->GetClientOfUserId(pEvent->GetInt("userid"));
			int oldteam = pEvent->GetInt("oldteam");
			int team = pEvent->GetInt("team");
			bool disconnect = pEvent->GetBool("disconnect");

			ApplyOnClientTeam(client, oldteam, team);
		}

		bool Mod_ZP::ApplyOnClientTeam(int id, int old_team, int new_team)
		{
			if (!sm::IsClientInGame(sm::IGamePlayerFrom(id)))
				return false;
			if (old_team == CS_TEAM_NONE || old_team == CS_TEAM_SPECTATOR)
			{
				if (new_team == teammgr::ZB_TEAM_HUMAN || new_team == teammgr::ZB_TEAM_ZOMBIE)
				{
					// If game round didn't start, then respawn
					if (m_iGameStatus < GAMESTATUS_INFECTIONSTART)
					{
						teammgr::Team_Set(id, teammgr::ZB_TEAM_HUMAN);
						// TODO : respawn
						MakeHuman(id);
					}
					else
					{
						teammgr::Team_Set(id, teammgr::ZB_TEAM_HUMAN);
						// TODO : respawn
						MakeZombie(id);
					}
				}
			}
			else if (old_team == teammgr::ZB_TEAM_HUMAN || old_team == teammgr::ZB_TEAM_ZOMBIE)
			{
				// block
				return true;
			}
			return false;
		}

		void Mod_ZP::OnTimer()
		{
			++m_iTimerSecs;

			if (m_iTimerSecs == 20 && m_iGameStatus == GAMESTATUS_ROUNDSTARTED)
			{
				SelectZombieOrigin();
				m_iGameStatus = GAMESTATUS_INFECTIONSTART;
			}
			CheckWinConditions();

			m_iTimerTask = sm::CreateTimerRAII(1.0, std::bind(&Mod_ZP::OnTimer, this));
		}

		void Mod_ZP::OnPlayerSpawnPost(CBaseEntity* player)
		{
			//sm::RequestFrame(&Mod_ZP::MakeHuman, this, sm::cbase2id(player));
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
			if (candidate == candidate_end)
				return;

			static std::random_device rd;
			std::shuffle(candidate, candidate_end, rd);

			auto iZombieAmount = std::distance(candidate, candidate_end) / 10 + 1;
			std::for_each(candidate, candidate + iZombieAmount, std::bind(&Mod_ZP::MakeZombie, this, std::placeholders::_1));
			std::for_each(candidate, candidate + iZombieAmount, std::bind(zombie::Originate, std::placeholders::_1, iZombieAmount, false));

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
					a.title = std::to_string(teammgr::TeamCount(teammgr::ZB_TEAM_HUMAN, true)) + " HUMAN\tVS\tZOMBIE " + std::to_string(teammgr::TeamCount(teammgr::ZB_TEAM_ZOMBIE, true));
				}
				return sm::Plugin_Continue;
			}

			return sm::Plugin_Continue;
		}

		void Mod_ZP::CheckWinConditions()
		{
			if (m_iGameStatus == GAMESTATUS_INFECTIONSTART)
			{
				if (!teammgr::TeamCount(teammgr::ZB_TEAM_ZOMBIE, true) || TimeExpired())
				{
					HumanWin();
				}
				else if (!teammgr::TeamCount(teammgr::ZB_TEAM_HUMAN, true))
				{
					ZombieWin();
				}
			}
		}

		void Mod_ZP::HumanWin()
		{
			alarm::AlarmPush({ alarm::ALARMTYPE_TIP, Color{100,150,255}, 5.0f, "人类战胜了僵尸...", "", "", [] {
				sm::cstrike::CS_TerminateRound(5.0f, CSRoundEnd_CTWin);
			}});
		}

		void Mod_ZP::ZombieWin()
		{
			alarm::AlarmPush({ alarm::ALARMTYPE_TIP, Color{255,150,100}, 5.0f, "僵尸统治了世界...", "", "", [] {
				sm::cstrike::CS_TerminateRound(5.0f, CSRoundEnd_TerroristWin);
			} });
		}

		bool Mod_ZP::TimeExpired() const
		{
			//if(auto cv = command::icvar->FindVar("mp_roundtime"))
			//	return cv->GetFloat() > 0.0f && m_iTimerSecs > cv->GetFloat() * 60;
			return false;
		}

		void Mod_ZP::MakeHuman(int id)
		{
			auto igp = sm::IGamePlayerFrom(id);
			if (!sm::IsClientConnected(igp))
				return;

			teammgr::Team_Set(id, teammgr::ZB_TEAM_HUMAN);
			zombie::Respawn(id, true);
			if(sm::IsPlayerAlive(igp))
				zmarket::ShowBuyMenu(id);
		}
		
		void Mod_ZP::MakeZombie(int id)
		{
			auto igp = sm::IGamePlayerFrom(id);
			if (!sm::IsClientConnected(igp))
				return;

			teammgr::Team_Set(id, teammgr::ZB_TEAM_ZOMBIE);
			if (sm::IsPlayerAlive(igp))
				zombie::Infect(id, 0, false);
		}

		void Mod_ZP::Event_OnRoundStart(IGameEvent* pEvent)
		{
			m_iGameStatus = GAMESTATUS_ROUNDSTARTED;
			
			for (int id = 1; id <= playerhelpers->GetMaxClients(); ++id)
			{
				auto igp = sm::IGamePlayerFrom(id);
				if (!sm::IsClientConnected(igp))
					continue;
				MakeHuman(id);
			}

			m_iTimerTask = sm::CreateTimerRAII(1.0, std::bind(&Mod_ZP::OnTimer, this));
			m_iTimerSecs = 0;
		}

		void Mod_ZP::Event_OnPlayerSpawn(IGameEvent* pEvent)
		{
			int id = pEvent->GetInt("userid");
		}

		void Mod_ZP::Event_OnRoundEnd(IGameEvent* pEvent)
		{
			int winner = pEvent->GetInt("winner");
			m_iGameStatus = GAMESTATUS_ROUNDEND;
		}
	}
}

