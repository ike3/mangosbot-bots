#ifndef _PlayerbotHelpMgr_H
#define _PlayerbotHelpMgr_H

#include "Common.h"
#include "PlayerbotAIBase.h"

#define BOT_HELP(name) sPlayerbotHelpMgr.GetBotText(name)

using namespace std;

struct BotHelpEntry
{
    BotHelpEntry() {}
    BotHelpEntry(std::string text, std::map<int32, std::string> text_locales) : m_text(text), m_text_locales(text_locales) {}
    std::string m_text;
    std::map<int32, std::string> m_text_locales;
};

class PlayerbotHelpMgr
{
    public:
        PlayerbotHelpMgr();
        virtual ~PlayerbotHelpMgr();
        static PlayerbotHelpMgr& instance()
        {
            static PlayerbotHelpMgr instance;
            return instance;
        }

	public:
        static void FormatHelpTopics(string& text);
        void LoadBotHelpTexts();

        string GetBotText(string name);
        bool GetBotText(string name, string& text);

    private:
        unordered_map<string, BotHelpEntry> botHelpText;
};

#define sPlayerbotHelpMgr PlayerbotHelpMgr::instance()

#endif
