#include "extension.h"
#include "sm/sourcemod.h"
#include "qqlogin.h"
#include "tools.h"

#include "../core/CellRecipientFilter.h"

#if SOURCE_ENGINE == SE_CSGO
// fix marco conflict from basetypes.h
#undef schema
#include <cstrike15_usermessages.pb.h>
#endif

namespace gameplay {
	namespace chat {

        void UTIL_SayText(CellRecipientFilter& crf, int ent_idx, const char* msg, bool chat)
        {
#if SOURCE_ENGINE == SE_CSGO || SOURCE_ENGINE == SE_BLADE
            assert(sm::g_SayTextMsg > 0);
			std::unique_ptr<CCSUsrMsg_SayText> pMsg = std::make_unique<CCSUsrMsg_SayText>();

			pMsg->set_ent_idx(ent_idx);
			pMsg->set_text(msg);
			pMsg->set_chat(chat);
            engine->SendUserMessage(crf, sm::g_SayTextMsg, *pMsg);
#else
            bf_write* pBitBuf = usermsgs->StartBitBufMessage(sm::g_SayTextMsg, players, 1, 0);
			pBitBuf->WriteByte(0);
			pBitBuf->WriteString(buffer);
			pBitBuf->WriteByte(1);
            usermsgs->EndMessage();
#endif
        }

        void UTIL_SayTextAll(int ent_idx, const char* msg, bool chat)
        {
            cell_t players[SM_MAXPLAYERS + 4];
            size_t count = 0;

            for (int id = 1; id <= playerhelpers->GetMaxClients(); ++id)
            {
                auto igp = sm::IGamePlayerFrom(id);
                if (!igp || !igp->IsConnected())
                    continue;
                players[count++] = id;
            }

            CellRecipientFilter crf;
            crf.Initialize(players, count);
            UTIL_SayText(crf, ent_idx, msg, chat);
        }

        namespace colorchat {
            constexpr const char normal = '\x01';
            constexpr const char red = '\x02';
            constexpr const char lightgreen = '\x03';
            constexpr const char darkgreen = '\x04';
            constexpr const char lightgreen2 = '\x05';
            constexpr const char green = '\x06';
            constexpr const char red2 = '\x07';
            constexpr const char gray = '\x08';
            constexpr const char lightorange = '\x09';
            constexpr const char lighterblue = '\x0A';
            constexpr const char lightblue = '\x0B';
            constexpr const char blue = '\x0C';
            constexpr const char purple = '\x0D';
            constexpr const char lightpurple = '\x0E';
            constexpr const char red3 = '\x0F';
            constexpr const char orange = '\x10';
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

                        UTIL_SayTextAll(id, (std::string() + " " + colorchat::lightgreen2 + "[" + tag + "] " + team_color + name + colorchat::normal + " : " + str).c_str(), true);
                        return true;
                    }
                }
            }
            return false;
		}
	}
}