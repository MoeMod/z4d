//
// Created by 梅峰源 on 2020-05-04.
//

#include "extension.h"
#include <filesystem.h>
#include "sm/sourcemod.h"

#include "mapmgr.h"
#include <string>

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
    namespace mapmgr {

        static std::vector<std::string> s_maps;
        static bool initialized = false;

        std::vector<std::string_view> GetServerMaps()
        {
            if(!initialized)
                Init();
            std::vector<std::string_view> ret(s_maps.size());
            std::copy(s_maps.begin(), s_maps.end(), ret.begin());
            return ret;
        }

        void Init()
        {
            s_maps.clear();
            FileFindHandle_t findHandle;
            for (const char *fileName = g_pFullFileSystem->FindFirstEx("maps/*.bsp", "GAME", &findHandle); fileName != nullptr; fileName = g_pFullFileSystem->FindNext(findHandle))
            {
                char buffer[PLATFORM_MAX_PATH];

                UTIL_StripExtension(fileName, buffer, sizeof(buffer));

                if (!gamehelpers->IsMapValid(buffer))
                    continue;

                s_maps.emplace_back(buffer);
            }
            initialized = true;
        }

    }
}