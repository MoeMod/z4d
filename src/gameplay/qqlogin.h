#pragma once

namespace gameplay {
    namespace qqlogin {

    void ShowAccountMenu(int id);
    void OnClientPreAdminCheck(int id);
    void Init();
    std::string GetUserTag(int id);
}

}