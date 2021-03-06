#pragma once

#include <Color.h>
#include <string>
#include "util/EventDispatcher.hpp"
#include "sm/hook_result.h"

namespace gameplay {
    namespace alarm {

        enum AlarmType_e
        {
            ALARMTYPE_NONE,
            ALARMTYPE_IDLE,
            ALARMTYPE_TIP,
            ALARMTYPE_INFECT,
            ALARMTYPE_KILL,
        };

        struct Alarm_s {
            AlarmType_e type;
            Color color;
            float time;
            std::string title;
            std::string subtitle;
            std::string sound;
            std::function<void()> callback;
        };

        void AlarmPush(const Alarm_s &alarm);
        void AlarmInsert(const Alarm_s &alarm);
        void TimerTip(float time, const std::string &text);
        void SendKillEvent(int killer, int victim, AlarmType_e iAlarmType);

        void Event_NewRound();

        cell_t x_alarm_push(IPluginContext *pContext, const cell_t *params);
        cell_t x_alarm_insert(IPluginContext *pContext, const cell_t *params);
        cell_t x_alarm_timertip(IPluginContext *pContext, const cell_t *params);
        cell_t x_alarm_kill(IPluginContext *pContext, const cell_t *params);

        extern EventDispatcher<sm::HookResult<void>(Alarm_s&)> g_fwAlarmShowPre;
        extern EventDispatcher<void(const Alarm_s&)> g_fwAlarmShowPost;
    }
}