#pragma once

namespace gameplay {
    namespace qqlogin {

    void ShowAccountMenu(int id);
    void OnClientPreAdminCheck(int id);
    void Init();
    const std::string& GetUserTag(int id);
}

}