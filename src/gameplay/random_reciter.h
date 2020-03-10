#pragma once

namespace gameplay {
    namespace random_reciter {

        void Event_NewRound();
        void Init();
        void OnClientCommand(edict_t *pEntity, const CCommand &args);
    }

}