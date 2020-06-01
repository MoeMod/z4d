//
// Created by 梅峰源 on 2020-03-10.
//

#include "extension.h"
#include "random_reciter.h"
#include "util/Reciter.h"
#include "sm/sourcemod.h"
#include "itemown.h"
#include "tools.h"

#include <future>

namespace gameplay {
    namespace random_reciter {

        static std::future<Reciter> g_NewReciter;
        static Reciter g_CurrentReciter;
        static bool g_bHasAnswered = false;

        void SetupNewReciter()
        {
            g_NewReciter = std::async(GetRandomReciter);
        }
        void Init()
        {
            SetupNewReciter();

        }

        void Event_NewRound()
        {
            try
            {
                g_CurrentReciter = g_NewReciter.get();
            }
            catch(const std::exception & e)
            {
                // ...
                // ignore it
                return;
            }

            g_bHasAnswered = false;
            sm::PrintToChatAll((" \x05[死神CS社区]\x01 抢答活动： \x02" + g_CurrentReciter.question + "\x01").c_str());

            SetupNewReciter();
        }

        void OnClientSay(int id, const CCommand& args, bool team)
        {
            if(g_bHasAnswered)
                return;

            std::string_view seq = args.ArgS();
            // 不知道为什么说话内容里面前后多了两个引号，直接去掉
            seq = seq.substr(1, seq.size() - 2);
            if(seq == g_CurrentReciter.answer)
            {
                g_bHasAnswered = true;
                int iAmount = std::max<int>(2, seq.size() / 4);
                sm::PrintToChatAll((" \x05[死神CS社区]\x01 恭喜 \x02" + std::string(sm::GetClientName(sm::IGamePlayerFrom(id))) + "\x01 成功抢答，获得"+ std::to_string(iAmount) + "个死神币奖励").c_str());

                itemown::ItemGive(id, "tz_coin", iAmount);
                
                /*
                auto entity = sm::id2cbase(id);
                assert(entity != nullptr);
                ++sm::EntProp<int>(entity, sm::Prop_Data, "m_iFrags");
                */
            }
        }
    }
}