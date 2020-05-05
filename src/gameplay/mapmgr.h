//
// Created by 梅峰源 on 2020-05-04.
//

#ifndef Z4D_MAPMGR_H
#define Z4D_MAPMGR_H

#include <vector>
#include <string_view>

namespace gameplay {
    namespace mapmgr {
        std::vector<std::string_view> GetServerMaps();

        void Init();
        void DelayedChangeLevel(const std::string &map);
    }
}


#endif //Z4D_MAPMGR_H
