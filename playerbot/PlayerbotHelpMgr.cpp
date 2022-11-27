#include "../botpch.h"
#include "playerbot.h"
#include "PlayerbotAIConfig.h"
#include "PlayerbotHelpMgr.h"

#include "DatabaseEnv.h"
#include "PlayerbotAI.h"

PlayerbotHelpMgr::PlayerbotHelpMgr()
{
}

PlayerbotHelpMgr::~PlayerbotHelpMgr()
{
}

void PlayerbotHelpMgr::FormatHelpTopics(string& text)
{
    //[h:subject:topic|name]
    size_t start_pos = 0;
    while ((start_pos = text.find("[h:", start_pos)) != std::string::npos) {
        size_t end_pos = text.find("]", start_pos);

        if (end_pos == std::string::npos)
            break;

        string oldLink = text.substr(start_pos, end_pos - start_pos + 1);

        if (oldLink.find("|") != std::string::npos)
        {
            string topicCode = oldLink.substr(3, oldLink.find("|") - 3);
            std::string topicName = oldLink.substr(oldLink.find("|") + 1);
            topicName.pop_back();

            if (topicCode.find(":") == std::string::npos)
                topicCode += ":" + topicName;

            string newLink = ChatHelper::formatHelpTopic(topicCode, topicName);

            text.replace(start_pos, oldLink.length(), newLink);
            start_pos += newLink.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
        }
        else
            start_pos= end_pos;
    }
}

void PlayerbotHelpMgr::LoadBotHelpTexts()
{
    sLog.outBasic("Loading playerbot texts...");
    QueryResult* results = PlayerbotDatabase.PQuery("SELECT `name`, `text`, `text_loc1`, `text_loc2`, `text_loc3`, `text_loc4`, `text_loc5`, `text_loc6`, `text_loc7`, `text_loc8` FROM `ai_playerbot_help_texts`");
    int count = 0;
    if (results)
    {
        do
        {
            std::string text;
            std::map<int32, std::string> text_locale;
            Field* fields = results->Fetch();
            string name = fields[0].GetString();
            text = fields[1].GetString();

            FormatHelpTopics(text);

            for (uint8 i = 1; i < MAX_LOCALE; ++i)
            {
                text_locale[sObjectMgr.GetStorageLocaleIndexFor(LocaleConstant(i))] = fields[i + 1].GetString();
                FormatHelpTopics(text_locale[sObjectMgr.GetStorageLocaleIndexFor(LocaleConstant(i))]);
            }

            botHelpText[name] = BotHelpEntry(text, text_locale);

            count++;
        } while (results->NextRow());

        delete results;
    }
    sLog.outBasic("%d playerbot helptexts loaded", count);
}

// general texts

string PlayerbotHelpMgr::GetBotText(string name)
{
    if (botHelpText.empty())
    {
        sLog.outError("Can't get bot help text %s! No bots help texts loaded!", name);
        return "";
    }
    if (botHelpText.find(name) == botHelpText.end())
    {
        sLog.outDetail("Can't get bot help text %s! No bots help texts for this name!", name);
        return "";
    }

    BotHelpEntry textEntry = botHelpText[name];
    int32 localePrio = sPlayerbotTextMgr.GetLocalePriority();
    if (localePrio == -1)
        return textEntry.m_text;
    else
    {
        if (!textEntry.m_text_locales[localePrio].empty())
            return textEntry.m_text_locales[localePrio];
        else
            return textEntry.m_text;
    }
}

bool PlayerbotHelpMgr::GetBotText(string name, string &text)
{
    text = GetBotText(name);
    return !text.empty();
}