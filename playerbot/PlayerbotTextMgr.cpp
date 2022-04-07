#include "../botpch.h"
#include "playerbot.h"
#include "PlayerbotAIConfig.h"
#include "PlayerbotTextMgr.h"

#include "DatabaseEnv.h"
#include "PlayerbotAI.h"

PlayerbotTextMgr::PlayerbotTextMgr()
{
    for (uint8 i = 1; i < MAX_LOCALE; ++i)
    {
        botTextLocalePriority[sObjectMgr.GetStorageLocaleIndexFor(LocaleConstant(i))] = 0;
    }
}

PlayerbotTextMgr::~PlayerbotTextMgr()
{
}

void PlayerbotTextMgr::replaceAll(std::string & str, const std::string & from, const std::string & to) {
    if (from.empty())
        return;
    size_t start_pos = 0;
    while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
    }
}

void PlayerbotTextMgr::LoadBotTexts()
{
    sLog.outBasic("Loading playerbot texts...");
    QueryResult* results = PlayerbotDatabase.PQuery("SELECT `name`, `text`, `say_type`, `reply_type`, `text_loc1`, `text_loc2`, `text_loc3`, `text_loc4`, `text_loc5`, `text_loc6`, `text_loc7`, `text_loc8` FROM `ai_playerbot_texts`");
    int count = 0;
    if (results)
    {
        do
        {
            std::map<uint32, std::string> text;
            Field* fields = results->Fetch();
            string name = fields[0].GetString();
            text[0] = fields[1].GetString();
            uint32 sayType = fields[2].GetUInt32();
            uint32 replyType = fields[3].GetUInt32();
            for (uint8 i = 1; i < MAX_LOCALE; ++i)
            {
                text[sObjectMgr.GetStorageLocaleIndexFor(LocaleConstant(i))] = fields[i + 3].GetString();
            }
            botTexts[name].push_back(BotTextEntry(name, text, sayType, replyType));
            count++;
        } while (results->NextRow());

        delete results;
    }
    sLog.outBasic("%d playerbot texts loaded", count);
}

void PlayerbotTextMgr::LoadBotTextChance()
{
    if (botTextChance.empty())
    {
        QueryResult* results = PlayerbotDatabase.PQuery("SELECT name, probability FROM ai_playerbot_texts_chance");
        if (results)
        {
            do
            {
                Field* fields = results->Fetch();
                string name = fields[0].GetString();
                uint32 probability = fields[1].GetUInt32();

                botTextChance[name] = probability;
            } while (results->NextRow());
            delete results;
        }
    }
}

// general texts

string PlayerbotTextMgr::GetBotText(string name)
{
    if (botTexts.empty())
    {
        sLog.outError("Can't get bot text %s! No bots texts loaded!", name);
        return "";
    }
    if (botTexts[name].empty())
    {
        sLog.outError("Can't get bot text %s! No bots texts for this name!", name);
        return "";
    }

    vector<BotTextEntry>& list = botTexts[name];
    BotTextEntry textEntry = list[urand(0, list.size() - 1)];
    uint32 localePrio = GetLocalePriority();
    if (localePrio > 0)
    {
        sLog.outBasic("Bot Text locale priority is %u", localePrio);
        sLog.outBasic("Bot Text for locale %u is %s", localePrio, textEntry.m_text[GetLocalePriority()]);
    }
    return !textEntry.m_text[GetLocalePriority()].empty() ? textEntry.m_text[GetLocalePriority()] : textEntry.m_text[0];
}

string PlayerbotTextMgr::GetBotText(string name, map<string, string> placeholders)
{
    string botText = GetBotText(name);
    if (botText.empty())
        return "";

    for (map<string, string>::iterator i = placeholders.begin(); i != placeholders.end(); ++i)
        replaceAll(botText, i->first, i->second);

    return botText;
}

// chat replies

string PlayerbotTextMgr::GetBotText(ChatReplyType replyType, map<string, string> placeholders)
{
    if (botTexts.empty())
    {
        sLog.outError("Can't get bot text reply %u! No bots texts loaded!", replyType);
        return "";
    }
    if (botTexts["reply"].empty())
    {
        sLog.outError("Can't get bot text reply %u! No bots texts replies!", replyType);
        return "";
    }

    vector<BotTextEntry>& list = botTexts["reply"];
    vector<BotTextEntry> proper_list;
    for (auto text : list)
    {
        if (text.m_replyType == replyType)
            proper_list.push_back(text);
    }

    BotTextEntry textEntry = proper_list[urand(0, proper_list.size() - 1)];
    string botText = !textEntry.m_text[GetLocalePriority()].empty() ? textEntry.m_text[GetLocalePriority()] : textEntry.m_text[0];

    for (map<string, string>::iterator i = placeholders.begin(); i != placeholders.end(); ++i)
        replaceAll(botText, i->first, i->second);

    return botText;
}

string PlayerbotTextMgr::GetBotText(ChatReplyType replyType, string name)
{
    map<string, string> placeholders;
    placeholders["%s"] = name;

    return GetBotText(replyType, placeholders);
}

// probabilities

bool PlayerbotTextMgr::rollTextChance(string name)
{
    if (!botTextChance[name])
        return true;

    return urand(0, 100) < botTextChance[name];
}

bool PlayerbotTextMgr::GetBotText(string name, string &text)
{
    if (!rollTextChance(name))
        return false;

    text = GetBotText(name);
    return !text.empty();
}

bool PlayerbotTextMgr::GetBotText(string name, string& text, map<string, string> placeholders)
{
    if (!rollTextChance(name))
        return false;

    text = GetBotText(name, placeholders);
    return !text.empty();
}

void PlayerbotTextMgr::AddLocalePriority(int32 locale)
{
    if (locale < 0)
        return;

    botTextLocalePriority[locale]++;
}

uint32 PlayerbotTextMgr::GetLocalePriority()
{
    uint32 topLocale = 0;

    // if no real players online, reset top locale
    if (!sWorld.GetActiveSessionCount())
    {
        ResetLocalePriority();
        return 0;
    }

    for (uint8 i = 1; i < MAX_LOCALE; ++i)
    {
        if (botTextLocalePriority[sObjectMgr.GetStorageLocaleIndexFor(LocaleConstant(i))] > topLocale)
            topLocale = i;
    }
    if (!topLocale)
        return 0;

    return sObjectMgr.GetStorageLocaleIndexFor(LocaleConstant(topLocale));
}

void PlayerbotTextMgr::ResetLocalePriority()
{
    for (uint8 i = 1; i < MAX_LOCALE; ++i)
    {
        botTextLocalePriority[sObjectMgr.GetStorageLocaleIndexFor(LocaleConstant(i))] = 0;
    }
}