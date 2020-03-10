//
// Created by 梅峰源 on 2020-03-10.
//

#include "extension.h"
#include "random_reciter.h"
#include "util/Reciter.h"
#include "sm/sourcemod.h"

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

        void OnClientCommand(edict_t *pEntity, const CCommand &args)
        {
            if(g_bHasAnswered)
                return;

            if(!strcmp(args[0], "say") || !strcmp(args[0], "say_team") )
            {
                std::string_view seq = args.ArgS();
                if(seq == g_CurrentReciter.answer)
                {
                    g_bHasAnswered = true;
                    sm::PrintToChatAll((" \x05[死神CS社区]\x01 恭喜 \x02" + g_CurrentReciter.question + "\x01 成功抢答。").c_str());
                    // TODO : 发奖励
                }
            }
        }


    }

}