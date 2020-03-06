
#define NOMINMAX
#include "extension.h"
#include "sm/sourcemod.h"
#include "alarm.h"

#include <list>
#include <array>

namespace gameplay {
    namespace alarm {

        static std::list<Alarm_s> g_AlarmList;

        static struct TimerTipInfo_s{
            float time = 0.0f;
            std::string text;
            float time_begin = 0.0f;
        } g_TimerTipInfo;

        void AlarmPush(const Alarm_s &alarm)
        {
            g_AlarmList.push_back(alarm);
        }

        void AlarmInsert(const Alarm_s &alarm)
        {
            g_AlarmList.push_front(alarm);
        }

        void TimerTip(float time, const std::string &text)
        {
            g_TimerTipInfo = { time, text, time };
        }

        enum AlarmKillerIndex_e
        {
            ALARM_KILL, ALARM_2KILL, ALARM_3KILL,
            ALARM_4KILL, ALARM_5KILL,
            ALARM_3KILLRECORD, ALARM_4KILLRECORD, ALARM_5KILLRECORD,
            ALARM_6KILLRECORD, ALARM_7KILLRECORD, ALARM_8KILLRECORD,
            ALARM_9KILLRECORD, ALARM_10KILLRECORD,
            ALARM_FIRSTBLOOD, ALARM_SHUTDOWN
        };

        constexpr const char *cfg_szTextAlarmKiller[] = {
            "KILL!" , "DOUBLE KILL!" , "TRIPLE KILL!" ,
            "QUADRA KILL!" , "PENTA KILL!" ,
            "KILLING SPREE! x3" , "DOMINATING! x4" , "MEGA KILL! x5" ,
            "UNSTOPPABLE! x6" , "WICKEDSICK! x7" , "MONSTER KILL! x8" ,
            "GOD LIKE! x9" , "Holy Shit! x10" ,
            "FIRST BLOOD" , "PERFECT!"
        };
        constexpr const char *cfg_szSoundAlarmKiller[] = {
            "" , "" , "" ,
            "" , "" ,
            "" , "" , "" ,
            "" , "" , "" ,
            "" , "" ,
            "" , ""
        };

        Color cfg_iAlarmColor[] = {
            { 255, 255, 255 },
            { 255, 255, 255 },
            { 200, 200, 50 },
            { 255, 42, 42 },
            { 42, 212, 255 }
        };

        struct PlayerKillInfo_s{
            int iRoundKill = 0;
            int iKillSeries = 0;
            int iKillingSpree = 0;
            float flLastKillTime = 0.f;
        };
        static bool g_bHasFirstBlood = false;
        std::array<PlayerKillInfo_s, SM_MAXPLAYERS + 1> g_ClientData;

        void SendKillEvent(int killer, int victim, AlarmType_e iAlarmType)
        {
            if(!killer || !victim)
                return;
            auto iAlarm = ALARM_KILL;
            g_ClientData[killer].iRoundKill++;
            if(sm::GetGameTime() - g_ClientData[killer].flLastKillTime > 11.5f)
                g_ClientData[killer].iKillSeries = 0;
            g_ClientData[killer].iKillSeries = std::min(g_ClientData[killer].iKillSeries + 1, 5);
            if(sm::GetGameTime() - g_ClientData[victim].flLastKillTime <= 11.5f)
                iAlarm = ALARM_SHUTDOWN;
            switch(g_ClientData[killer].iKillingSpree)
            {
                case 3:iAlarm = ALARM_3KILLRECORD;break;
                case 4:iAlarm = ALARM_4KILLRECORD;break;
                case 5:iAlarm = ALARM_5KILLRECORD;break;
                case 6:iAlarm = ALARM_6KILLRECORD;break;
                case 7:iAlarm = ALARM_7KILLRECORD;break;
                case 8:iAlarm = ALARM_8KILLRECORD;break;
                case 9:iAlarm = ALARM_9KILLRECORD;break;
                case 10:iAlarm = ALARM_10KILLRECORD;break;
                default:break;
            }
            switch(g_ClientData[killer].iKillSeries)
            {
                case 2:iAlarm = ALARM_2KILL;break;
                case 3:iAlarm = ALARM_3KILL;break;
                case 4:iAlarm = ALARM_4KILL;break;
                case 5:iAlarm = ALARM_5KILL;break;
                default:break;
            }
            g_ClientData[killer].flLastKillTime = sm::GetGameTime();
            if(!std::exchange(g_bHasFirstBlood, true))
                iAlarm = ALARM_FIRSTBLOOD;

            g_ClientData[victim].iKillingSpree = 0;
            g_ClientData[killer].iKillingSpree++;

            AlarmPush(Alarm_s{iAlarmType, cfg_iAlarmColor[iAlarmType], 1.0f,
                              sm::GetClientName(killer) + std::string("\t:\t") + cfg_szTextAlarmKiller[iAlarm],
                              sm::GetClientName(killer) + std::string(" has killed ") + sm::GetClientName(victim),
                              ""
                              });
        }

        std::shared_ptr<void> g_taskAlarm;

        template<class Fn> std::shared_ptr<ITimer> SetTask(float fInterval, Fn &&onTick, int flags = 0)
        {
            class MyHandler : public ITimedEvent {
            public:
                explicit MyHandler(Fn fn, ITimer*p = nullptr) : m_fn(std::move(fn)), m_pTimer(p) {}
                ResultType OnTimer(ITimer *pTimer, void *pData) override
                {
                    m_fn();
                    return Pl_Continue;
                }
                void OnTimerEnd(ITimer *pTimer, void *pData) override
                {
                    // delete this !
                    assert(m_pSharedThis.get() == this);
                    auto spThis = std::exchange(m_pSharedThis, nullptr);
                }
                ~MyHandler()
                {
                    assert(m_pTimer != nullptr);
                    if(m_pSharedThis != nullptr)
                        timersys->KillTimer(m_pTimer);
                }
                Fn m_fn;
                ITimer* m_pTimer;
                std::shared_ptr<MyHandler> m_pSharedThis;
            };
            auto handler = std::make_shared<MyHandler>(std::forward<Fn>(onTick));
            handler->m_pTimer = timersys->CreateTimer(handler.get(), fInterval, nullptr, flags);
            handler->m_pSharedThis = handler; // self-cycle
            return std::shared_ptr<ITimer>(handler, handler->m_pTimer);
        }

        void CheckAlarmTask()
        {
            Alarm_s alarm = Alarm_s{ALARMTYPE_IDLE, cfg_iAlarmColor[ALARMTYPE_IDLE], 1.0f};
            if(!g_AlarmList.empty())
            {
                alarm = g_AlarmList.front();
                g_AlarmList.pop_front();
            }

            int iTimerCount = static_cast<int>(g_TimerTipInfo.time_begin + g_TimerTipInfo.time - sm::GetGameTime());
            if(iTimerCount > 0)
            {
                alarm = Alarm_s{ALARMTYPE_TIP, cfg_iAlarmColor[ALARMTYPE_TIP], 1.0f, g_TimerTipInfo.text + " [" + std::to_string(iTimerCount) + "]"};
            }

            if (!alarm.title.empty() || !alarm.subtitle.empty())
            {
                sm::hud_text_parms textparms = {};
                auto color = std::make_tuple(alarm.color.r(), alarm.color.g(), alarm.color.b());
                std::tie(textparms.r1, textparms.g1, textparms.b1) = color;
                std::tie(textparms.r2, textparms.g2, textparms.b2) = color;
                textparms.effect = 0;
                textparms.x = -1.0f;
                textparms.y = 0.15f + 0.05f;
                textparms.fxTime = 1.0f;
                textparms.holdTime = alarm.time + 0.1f;
                textparms.fadeinTime = 0.1f;
                textparms.fadeoutTime = 0.4f;
                textparms.channel = 3;

                for (int id = 1; id <= playerhelpers->GetMaxClients(); ++id)
                {
                    if (!playerhelpers->GetGamePlayer(id)->IsConnected())
                        continue;
                    sm::UTIL_SendHudText(id, textparms, alarm.title.c_str());
                }
            }

            g_taskAlarm = SetTask(alarm.time, CheckAlarmTask);
        }

        void Event_NewRound()
        {
            g_ClientData = {};
            g_TimerTipInfo = {};
            g_bHasFirstBlood = false;
            CheckAlarmTask();
        }
    }
}