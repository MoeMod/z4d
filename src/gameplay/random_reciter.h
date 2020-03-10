#pragma once

namespace gameplay {
    namespace random_reciter {

        void Event_NewRound();
        void Init();
        void OnClientSay(int id, const CCommand& command, bool team);
    }

}