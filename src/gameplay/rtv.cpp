//
// Created by 梅峰源 on 2020-03-11.
//

#include "extension.h"
#include <filesystem.h>
#include "rtv.h"
#include "util/smhelper.h"
#include "sm/sourcemod.h"

#include <string>
#include <vector>
#include <bitset>
#include <algorithm>
#include <util/smhelper.h>

extern IFileSystem *g_pFullFileSystem;

#define PATHSEPARATOR(c) ((c) == '\\' || (c) == '/')
static void UTIL_StripExtension(const char *in, char *out, int outSize)
{
    // Find the last dot. If it's followed by a dot or a slash, then it's part of a
    // directory specifier like ../../somedir/./blah.

    // scan backward for '.'
    int end = strlen(in) - 1;
    while (end > 0 && in[end] != '.' && !PATHSEPARATOR(in[end]))
    {
        --end;
    }

    if (end > 0 && !PATHSEPARATOR(in[end]) && end < outSize)
    {
        int nChars = MIN(end, outSize-1);
        if (out != in)
        {
            memcpy(out, in, nChars);
        }
        out[nChars] = 0;
    }
    else
    {
        // nothing found
        if (out != in)
        {
            strncpy(out, in, outSize);
        }
    }
}

namespace gameplay {
    namespace rtv {

        struct Selection{
            std::string mapname;
            std::bitset<SM_MAXPLAYERS + 1> selected;
        };
        bool operator<(const Selection & s1, const Selection & s2){
            return s1.selected.count() > s2.selected.count();
        }
        std::vector<Selection> g_SelectedMaps;
        std::shared_ptr<ITimer> g_ChangelevelTask;

        void Init()
        {
            g_SelectedMaps.clear();
            g_ChangelevelTask = nullptr;
            FileFindHandle_t findHandle;
            for (const char *fileName = g_pFullFileSystem->FindFirstEx("maps/*.bsp", "GAME", &findHandle); fileName != nullptr; fileName = g_pFullFileSystem->FindNext(findHandle))
            {
                char buffer[PLATFORM_MAX_PATH];

                UTIL_StripExtension(fileName, buffer, sizeof(buffer));

                if (!gamehelpers->IsMapValid(buffer))
                    continue;

                g_SelectedMaps.push_back({buffer, {}});
            }
        }

        void CheckRTV()
        {
            assert(std::is_sorted(g_SelectedMaps.begin(), g_SelectedMaps.end()));
            assert(!g_SelectedMaps.empty());
            Selection &first = g_SelectedMaps.front();
            if(first.selected.count() >= playerhelpers->GetNumPlayers() / 2)
            {
                sm::PrintToChatAll((std::string() + " \x05[死神CS社区]\x01 由于服务器有超过一半的人钦点\x02" + first.mapname + "\x01 即将更换地图...").c_str());
                g_ChangelevelTask = util::SetTask(5.0, [mapName = first.mapname]{ engine->ServerCommand(("changelevel " + mapName + "\n").c_str()); g_ChangelevelTask = nullptr; });
                
            }
        }

        void OnSayRTV(int id)
        {
            if (g_ChangelevelTask)
                return;
            auto menu = util::MakeMenu([](IBaseMenu* menu, int id, unsigned int item) {

                const char *map = menu->GetItemInfo(item, nullptr);

                for(Selection &s : g_SelectedMaps)
                {
                    s.selected.set(id, false);
                }

                auto iter = std::find_if(g_SelectedMaps.begin(), g_SelectedMaps.end(), [map](const Selection&s){ return s.mapname == map; });
                assert(iter != g_SelectedMaps.end());

                iter->selected.set(id, true);

                sm::PrintToChatAll((std::string() + " \x05[死神CS社区]\x01\x02" + sm::GetClientName(sm::IGamePlayerFrom(id)) + "\x01钦点了 \x02" + map + "\x01 (还差" + std::to_string(playerhelpers->GetNumPlayers() / 2 - iter->selected.count()) + "人) => say rtv并选择你想钦点的地图~~~").c_str());

                // 重新排序，把提名率高的地图放在前面（注意：迭代器失效）
                std::sort(g_SelectedMaps.begin(), g_SelectedMaps.end());

                CheckRTV();
            });

            menu->RemoveAllItems();
            menu->SetDefaultTitle("钦点地图 / RTV");

            assert(std::is_sorted(g_SelectedMaps.begin(), g_SelectedMaps.end()));
            for(const auto &[map, selected] : g_SelectedMaps)
            {
                menu->AppendItem(map.c_str(), ItemDrawInfo((map + "(" + std::to_string(selected.count() * 100 / playerhelpers->GetNumPlayers()) + "%)").c_str()));
            }

            menu->SetMenuOptionFlags(menu->GetMenuOptionFlags() | MENUFLAG_BUTTON_EXIT);
            menu->Display(id, MENU_TIME_FOREVER);
        }
    }
}