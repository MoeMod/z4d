#include "extension.h"
#include "sm/sourcemod.h"
#include "sm/ranges.h"
#include "qqlogin.h"
#include "tools.h"
#include "csgo_colorchat.h"

#include "../core/CellRecipientFilter.h"

namespace gameplay {
	namespace chat {
        void UTIL_SayTextAll(int ent_idx, const char* msg, bool chat)
        {
            sm::SendSayText(sm::ranges::Players() | sm::ranges::Connected(), ent_idx, msg, chat);
        }

        bool OnClientSay(int id, std::string str, bool team)
        {
            if (!team && !str.empty())
            {
                if (!(str[0] == '!') && !(str[0] == '\"' && str[1] == '!'))
                {
                    if (str.front() == '\"' && str.back() == '\"')
                        str = str.substr(1, str.size() - 2);

                    if (auto tag = qqlogin::GetUserTag(id); !tag.empty() && !str.empty())
                    {
                        auto name = sm::GetClientName(sm::IGamePlayerFrom(id));

                        char team_color = colorchat::gray;
                        switch (tools::GetTeam(sm::id2cbase(id)))
                        {
                            case CS_TEAM_T:
                                team_color = colorchat::lightorange;
                                break;
                            case CS_TEAM_CT:
                                team_color = colorchat::lighterblue;
                                break;
                            default:
                                team_color = colorchat::gray;
                                break;
                        }

                        UTIL_SayTextAll(id, (std::string() + " " + qqlogin::GetUserTagColor(id) + "[" + tag + "] " + team_color + name + colorchat::normal + " : " + str).c_str(), true);
                        return true;
                    }
                }
            }
            return false;
        }
	}
}