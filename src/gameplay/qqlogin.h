#pragma once

#include "sm/coro.h"

namespace gameplay {
    namespace qqlogin {

    sm::coro::Task ShowAccountMenu(int id);
    void OnClientPreAdminCheck(int id);
    void Init();
    std::string GetUserTag(int id);
    char GetUserTagColor(int id);
}

}