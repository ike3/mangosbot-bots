#include "../botpch.h"
#include "playerbot.h"
#include "ChatFilter.h"
#include "strategy/values/RtiTargetValue.h"

using namespace ai;
using namespace std;

string ChatFilter::Filter(string message)
{
    if (message.find("@") == string::npos)
        return message;

    return message.substr(message.find(" ") + 1);
}

class StrategyChatFilter : public ChatFilter
{
public:
    StrategyChatFilter(PlayerbotAI* ai) : ChatFilter(ai) {}

#ifndef GenerateBotHelp
    virtual string GetHelpName() {
        return "strategy";
    }
    virtual unordered_map<string,string> GetFilterExamples() 
    {
        unordered_map<string, string> retMap;
        retMap["@tank"] = "All bots that have a tank spec.";
        retMap["@dps"] = "All bots that do not have a tank or healing spec.";
        retMap["@heal"] = "All bots that have a healing spec.";
        retMap["@notank"] = "All bots that do not have a tank spec.";
        retMap["@nodps"] = "All bots that have a tank or healing spec.";
        retMap["@noheal"] = "All bots that do not have a healing spec.";
        retMap["@ranged"] = "All bots that use ranged attacks.";
        retMap["@melee"] = "All bots that use melee attacks.";
        return retMap;
    }
    virtual string GetHelpDescription() {
        return "This filter selects bots with a specific role or weapon range.";
    }
#endif

    virtual string Filter(string message)
    {
        Player* bot = ai->GetBot();

        bool tank = message.find("@tank") == 0;
        if (tank && !ai->IsTank(bot))
            return "";

        bool dps = message.find("@dps") == 0;
        if (dps && (ai->IsTank(bot) || ai->IsHeal(bot)))
            return "";

        bool heal = message.find("@heal") == 0;
        if (heal && !ai->IsHeal(bot))
            return "";

        bool tank = message.find("@notank") == 0;
        if (tank && ai->IsTank(bot))
            return "";

        bool dps = message.find("@nodps") == 0;
        if (dps && !ai->IsTank(bot) && !ai->IsHeal(bot))
            return "";

        bool heal = message.find("@noheal") == 0;
        if (heal && ai->IsHeal(bot))
            return "";

        bool ranged = message.find("@ranged") == 0;
        if (ranged && !ai->IsRanged(bot))
            return "";

        bool melee = message.find("@melee") == 0;
        if (melee && ai->IsRanged(bot))
            return "";

        if (tank || dps || heal || ranged || melee)
            return ChatFilter::Filter(message);

        return message;
    }
};

class LevelChatFilter : public ChatFilter
{
public:
    LevelChatFilter(PlayerbotAI* ai) : ChatFilter(ai) {}

#ifndef GenerateBotHelp
    virtual string GetHelpName() {
        return "level";
    }
    virtual unordered_map<string, string> GetFilterExamples()
    {
        unordered_map<string, string> retMap;
        retMap["@60"] = "All bots that are level 60.";
        retMap["@10-20"] = "All bots ranging from level 10 to 20.";
        return retMap;
    }
    virtual string GetHelpDescription() {
        return "This filter selects bots based on level.";
    }
#endif

    virtual string Filter(string message)
    {
        Player* bot = ai->GetBot();

        if (message[0] != '@')
            return message;

        if (message.find("-") != string::npos)
        {
            uint32 fromLevel = atoi(message.substr(message.find("@") + 1, message.find("-")).c_str());
            uint32 toLevel = atoi(message.substr(message.find("-") + 1, message.find(" ")).c_str());

            if (bot->GetLevel() >= fromLevel && bot->GetLevel() <= toLevel)
                return ChatFilter::Filter(message);

            return message;
        }

        uint32 level = atoi(message.substr(message.find("@") + 1, message.find(" ")).c_str());
        if (bot->GetLevel() == level)
            return ChatFilter::Filter(message);

        return message;
    }
};

class CombatTypeChatFilter : public ChatFilter
{
public:
    CombatTypeChatFilter(PlayerbotAI* ai) : ChatFilter(ai) {}

#ifndef GenerateBotHelp
    virtual string GetHelpName() {
        return "combat";
    }
    virtual unordered_map<string, string> GetFilterExamples()
    {
        unordered_map<string, string> retMap;
        retMap["@ranged"] = "All bots that use ranged attacks.";
        retMap["@melee"] = "All bots that use melee attacks.";
        return retMap;
    }
    virtual string GetHelpDescription() {
        return "This filter selects bots with a specific weapon range.";
    }
#endif

    virtual string Filter(string message)
    {
        Player* bot = ai->GetBot();

        bool melee = message.find("@melee") == 0;
        bool ranged = message.find("@ranged") == 0;

        if (!melee && !ranged)
            return message;

        switch (bot->getClass())
        {
            case CLASS_WARRIOR:
            case CLASS_PALADIN:
            case CLASS_ROGUE:
            /*case CLASS_DEATH_KNIGHT:
                if (ranged)
                    return "";
                break;*/

            case CLASS_HUNTER:
            case CLASS_PRIEST:
            case CLASS_MAGE:
            case CLASS_WARLOCK:
                if (melee)
                    return "";
                break;

            case CLASS_DRUID:
                if (ranged && ai->IsTank(bot))
                    return "";
                if (melee && !ai->IsTank(bot))
                    return "";
                break;

            case CLASS_SHAMAN:
                if (melee && ai->IsHeal(bot))
                    return "";
                if (ranged && !ai->IsHeal(bot))
                    return "";
                break;
        }

        return ChatFilter::Filter(message);
    }
};

class RtiChatFilter : public ChatFilter
{
public:
#ifndef GenerateBotHelp
    virtual string GetHelpName() {
        return "rti";
    }
    virtual unordered_map<string, string> GetFilterExamples()
    {
        unordered_map<string, string> retMap;
        retMap["@star"] = "All bots that are marked with or are targeing something marked with star.";
        retMap["@circle"] = "All bots that are marked with or are targeing something marked with star.";
        return retMap;
    }
    virtual string GetHelpDescription() {
        return "This filter selects bots that are marked with or are targeting sonething marked with a raid target icon.";
    }
#endif

    RtiChatFilter(PlayerbotAI* ai) : ChatFilter(ai)
    {
        rtis.push_back("@star");
        rtis.push_back("@circle");
        rtis.push_back("@diamond");
        rtis.push_back("@triangle");
        rtis.push_back("@moon");
        rtis.push_back("@square");
        rtis.push_back("@cross");
        rtis.push_back("@skull");
    }

    virtual string Filter(string message)
    {
        Player* bot = ai->GetBot();
        Group *group = bot->GetGroup();
        if(!group)
            return message;

        bool found = false;
        bool isRti = false;
        for (list<string>::iterator i = rtis.begin(); i != rtis.end(); i++)
        {
            string rti = *i;

            bool isRti = message.find(rti) == 0;
            if (!isRti)
                continue;

            ObjectGuid rtiTarget = group->GetTargetIcon(RtiTargetValue::GetRtiIndex(rti.substr(1)));
            if (bot->GetObjectGuid() == rtiTarget)
                return ChatFilter::Filter(message);

            Unit* target = *ai->GetAiObjectContext()->GetValue<Unit*>("current target");
            if (!target)
                return "";

            if (target->GetObjectGuid() != rtiTarget)
                return "";

            found |= isRti;
            if (found)
                break;
        }

        if (found)
            return ChatFilter::Filter(message);

        return message;
    }

private:
    list<string> rtis;
};

class ClassChatFilter : public ChatFilter
{
public:
    ClassChatFilter(PlayerbotAI* ai) : ChatFilter(ai)
    {
#ifdef MANGOSBOT_TWO
        classNames["@death_knight"] = CLASS_DEATH_KNIGHT;
#endif
        classNames["@druid"] = CLASS_DRUID;
        classNames["@hunter"] = CLASS_HUNTER;
        classNames["@mage"] = CLASS_MAGE;
        classNames["@paladin"] = CLASS_PALADIN;
        classNames["@priest"] = CLASS_PRIEST;
        classNames["@rogue"] = CLASS_ROGUE;
        classNames["@shaman"] = CLASS_SHAMAN;
        classNames["@warlock"] = CLASS_WARLOCK;
        classNames["@warrior"] = CLASS_WARRIOR;
    }

#ifndef GenerateBotHelp
    virtual string GetHelpName() {
        return "class";
    }
    virtual unordered_map<string, string> GetFilterExamples()
    {
        unordered_map<string, string> retMap;
        retMap["@rogue"] = "All rogue bots.";
        retMap["@warlock"] = "All warlock bots.";
        return retMap;
    }
    virtual string GetHelpDescription() {
        return "This filter selects bots have a certain class.";
    }
#endif

    virtual string Filter(string message)
    {
        Player* bot = ai->GetBot();

        bool found = false;
        bool isClass = false;
        for (map<string, uint8>::iterator i = classNames.begin(); i != classNames.end(); i++)
        {
            bool isClass = message.find(i->first) == 0;
            if (isClass && bot->getClass() != i->second)
                return "";

            found |= isClass;
            if (found)
                break;
        }

        if (found)
            return ChatFilter::Filter(message);

        return message;
    }

private:
    map<string, uint8> classNames;
};

class GroupChatFilter : public ChatFilter
{
public:
    GroupChatFilter(PlayerbotAI* ai) : ChatFilter(ai) {}

#ifndef GenerateBotHelp
    virtual string GetHelpName() {
        return "group";
    }
    virtual unordered_map<string, string> GetFilterExamples()
    {
        unordered_map<string, string> retMap;
        retMap["@group"] = "All bots in a group.";
        retMap["@group2"] = "All bots in group 2.";
        retMap["@group4-6"] = "All bots in group 4 to 6.";
        retMap["@nogroup"] = "All bots that are not grouped.";
        retMap["@leader"] = "All bots that are leader of their group.";
        retMap["@raid"] = "All bots that are in a raid group.";
        retMap["@noraid"] = "All bots that are not in a raid group.";
        retMap["@rleader"] = "All bots that are leader of a raid group.";
        return retMap;
    }
    virtual string GetHelpDescription() {
        return "This filter selects bots based on their group.";
    }
#endif

    virtual string Filter(string message)
    {
        Player* bot = ai->GetBot();

        if (message.find("@group") == 0)
        {
            string pnum = message.substr(6, message.find(" ")-6);
            int from = atoi(pnum.c_str());
            int to = from;
            if (pnum.find("-") != string::npos)
            {
                from = atoi(pnum.substr(pnum.find("@") + 1, pnum.find("-")).c_str());
                to = atoi(pnum.substr(pnum.find("-") + 1, pnum.find(" ")).c_str());
            }

            if (!bot->GetGroup())
                return message;

            int sg = (int)bot->GetSubGroup() + 1;
            if (sg >= from && sg <= to)
                return ChatFilter::Filter(message);
        }
        if (message.find("@nogroup") == 0)
        {
            if (bot->GetGroup())
                return message;

            return ChatFilter::Filter(message);
        }
        if (message.find("@leader") == 0)
        {
            if (!bot->GetGroup())
                return message;

            if(bot->GetGroup()->IsLeader(bot->GetObjectGuid()))
                return ChatFilter::Filter(message);
        }
        if (message.find("@raid") == 0)
        {
            if (!bot->GetGroup() || !bot->GetGroup()->IsRaidGroup())
                return message;

            return ChatFilter::Filter(message);
        }
        if (message.find("@noraid") == 0)
        {
            if (bot->GetGroup() && bot->GetGroup()->IsRaidGroup())
                return message;

            return ChatFilter::Filter(message);
        }
        if (message.find("@rleader") == 0)
        {
            if (!bot->GetGroup())
                return message;

            if (!bot->GetGroup()->IsRaidGroup())
                return message;

            if (bot->GetGroup()->IsLeader(bot->GetObjectGuid()))
                return ChatFilter::Filter(message);
        }

        return message;
    }
};

class GuildChatFilter : public ChatFilter
{
public:
    GuildChatFilter(PlayerbotAI* ai) : ChatFilter(ai) {}

#ifndef GenerateBotHelp
    virtual string GetHelpName() {
        return "guild";
    }
    virtual unordered_map<string, string> GetFilterExamples()
    {
        unordered_map<string, string> retMap;
        retMap["@guild"] = "All bots in a guild.";
        retMap["@guild=raiders"] = "All bots in guild raiders.";
        retMap["@noguild"] = "All bots not in a guild.";
        retMap["@gleader"] = "All bots that are a guild leader.";
        retMap["@grank=Initiate"] = "All bots that have rank Initiate in their guild.";
        return retMap;
    }
    virtual string GetHelpDescription() {
        return "This filter selects bots based on their guild.";
    }
#endif

    virtual string Filter(string message)
    {
        Player* bot = ai->GetBot();

        if (message.find("@guild=") == 0)
        {
            if (!bot->GetGuildId())
                return message;

            string pguild = message.substr(7, message.find(" ")-7);

            if (!pguild.empty())
            {
                Guild* guild = sGuildMgr.GetGuildById(bot->GetGuildId());
                string guildName = guild->GetName();

                if (pguild.find(guildName) != 0)
                    return message;
            }

            return ChatFilter::Filter(message);
        }
        if (message.find("@guild") == 0)
        {
            if (!bot->GetGuildId())
                return message;

            string pguild = message.substr(6, message.find(" "));
            
            if (!pguild.empty())
            {
                Guild* guild = sGuildMgr.GetGuildById(bot->GetGuildId());
                string guildName = guild->GetName();

                if (pguild.find(guildName) == 0)
                    return message;
            }

            return ChatFilter::Filter(message);
        }
        if (message.find("@noguild") == 0)
        {
            if (bot->GetGuildId())
                return message;

            return ChatFilter::Filter(message);
        }
        if (message.find("@gleader") == 0)
        {
            if (!bot->GetGuildId())
                return message;

            Guild* guild = sGuildMgr.GetGuildById(bot->GetGuildId());
            if (guild->GetLeaderGuid() != bot->GetObjectGuid())
                return message;

            return ChatFilter::Filter(message);
        }
        if (message.find("@grank=") == 0)
        {
            if (!bot->GetGuildId())
                return message;

            string rank = message.substr(7, message.find(" ")-7);

            if (!rank.empty())
            {
                Guild* guild = sGuildMgr.GetGuildById(bot->GetGuildId());
                string rankName = guild->GetRankName(guild->GetRank(bot->GetObjectGuid()));

                if (rank.find(rankName) != 0)
                    return message;
            }

            return ChatFilter::Filter(message);
        }

        return message;
    }
};

class StateChatFilter : public ChatFilter
{
public:
    StateChatFilter(PlayerbotAI* ai) : ChatFilter(ai) {}

#ifndef GenerateBotHelp
    virtual string GetHelpName() {
        return "state";
    }
    virtual unordered_map<string, string> GetFilterExamples()
    {
        unordered_map<string, string> retMap;
        retMap["@needrepair"] = "All bots that have durability below 20%.";
        retMap["@outside"] = "All bots that are outside of an instance.";
        retMap["@inside"] = "All bots that are inside an instance.";
        return retMap;
    }
    virtual string GetHelpDescription() {
        return "This filter selects bots based on their state.";
    }
#endif

    virtual string Filter(string message)
    {
        Player* bot = ai->GetBot();
        AiObjectContext* context = bot->GetPlayerbotAI()->GetAiObjectContext();

        if (message.find("@needrepair") == 0)
        {
            if (AI_VALUE(uint8, "durability") > 20)
                return message;
          
            return ChatFilter::Filter(message);
        }
        if (message.find("@outside") == 0)
        {
            if (!WorldPosition(bot).isOverworld())
                return message;

            return ChatFilter::Filter(message);
        }
        if (message.find("@inside") == 0)
        {
            if (WorldPosition(bot).isOverworld())
                return message;

            return ChatFilter::Filter(message);
        }        

        return message;
    }
};

CompositeChatFilter::CompositeChatFilter(PlayerbotAI* ai) : ChatFilter(ai)
{
    filters.push_back(new StrategyChatFilter(ai));
    filters.push_back(new ClassChatFilter(ai));
    filters.push_back(new RtiChatFilter(ai));
    filters.push_back(new CombatTypeChatFilter(ai));
    filters.push_back(new LevelChatFilter(ai));
    filters.push_back(new GroupChatFilter(ai));
    filters.push_back(new GuildChatFilter(ai));
    filters.push_back(new StateChatFilter(ai));
}

CompositeChatFilter::~CompositeChatFilter()
{
    for (list<ChatFilter*>::iterator i = filters.begin(); i != filters.end(); i++)
        delete (*i);
}

string CompositeChatFilter::Filter(string message)
{
    for (int j = 0; j < filters.size(); ++j)
    {
        for (list<ChatFilter*>::iterator i = filters.begin(); i != filters.end(); i++)
        {
            message = (*i)->Filter(message);
            if (message.empty())
                break;
        }
    }

    return message;
}

