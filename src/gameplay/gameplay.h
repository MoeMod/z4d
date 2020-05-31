//
// Created by 梅峰源 on 2020-02-27.
//

class IGameEvent;

namespace gameplay {
    bool SDK_OnLoad(char *error, size_t maxlength, bool late);
    void SDK_OnUnload();

    void Event_OnRoundStart(IGameEvent *pEvent);
    void Event_OnPlayerSpawn(IGameEvent *pEvent);
    void Event_OnPlayerDeath(IGameEvent *pEvent);
    void OnClientPreAdminCheck(int id);
    void OnClientPostAdminCheck(int id);
    void OnClientPutInServer(int id);
    void OnClientDisconnected(int id);
    bool OnClientCommand(edict_t *pEntity, const CCommand &args);
    bool OnClientSay(int id, const CCommand& command, bool team);
}