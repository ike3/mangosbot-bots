#include "../botpch.h"
#include "playerbot.h"
#include "PlayerbotAIConfig.h"
#include "PlayerbotFactory.h"
#include "PlayerbotDbStore.h"
#include <cstdlib>
#include <iostream>

#include "LootObjectStack.h"
#include "strategy/values/Formations.h"
#include "strategy/values/PositionValue.h"
INSTANTIATE_SINGLETON_1(PlayerbotDbStore);

using namespace std;
using namespace ai;

void PlayerbotDbStore::Load(PlayerbotAI *ai)
{
    uint64 guid = ai->GetBot()->GetObjectGuid().GetRawValue();
    uint32 account = sObjectMgr.GetPlayerAccountIdByGUID(ObjectGuid(guid));
    if (sPlayerbotAIConfig.IsInRandomAccountList(account))
        return;

    QueryResult* results = CharacterDatabase.PQuery("SELECT `key`,`value` FROM `ai_playerbot_db_store` WHERE `guid` = '%u'", guid);
    if (results)
    {
        ai->ClearStrategies(BOT_STATE_COMBAT);
        ai->ClearStrategies(BOT_STATE_NON_COMBAT);
        ai->ChangeStrategy("+chat", BOT_STATE_COMBAT);
        ai->ChangeStrategy("+chat", BOT_STATE_NON_COMBAT);

        list<string> values;
        do
        {
            Field* fields = results->Fetch();
            string key = fields[0].GetString();
            string value = fields[1].GetString();
            if (key == "value")
            {
                values.push_back(value);
            }
            else
            {
                ExternalEventHelper helper(ai->GetAiObjectContext());
                helper.ParseChatCommand(value, ai->GetMaster());
                ai->DoNextAction();
            }
        } while (results->NextRow());

        ai->GetAiObjectContext()->Load(values);
        delete results;
    }
}

void PlayerbotDbStore::Save(PlayerbotAI *ai)
{
    uint64 guid = ai->GetBot()->GetObjectGuid().GetRawValue();
    uint32 account = sObjectMgr.GetPlayerAccountIdByGUID(ObjectGuid(guid));
    if (sPlayerbotAIConfig.IsInRandomAccountList(account))
        return;

    Reset(ai);

    list<string> data = ai->GetAiObjectContext()->Save();
    for (list<string>::iterator i = data.begin(); i != data.end(); ++i)
    {
        SaveValue(guid, "value", *i);
    }

    SaveValue(guid, "co", FormatStrategies("co", ai->GetStrategies(BOT_STATE_COMBAT)));
    SaveValue(guid, "nc", FormatStrategies("nc", ai->GetStrategies(BOT_STATE_NON_COMBAT)));
    SaveValue(guid, "dead", FormatStrategies("dead", ai->GetStrategies(BOT_STATE_DEAD)));
}

string PlayerbotDbStore::FormatStrategies(string type, list<string> strategies)
{
    ostringstream out;
    out << type << " ";
    for(list<string>::iterator i = strategies.begin(); i != strategies.end(); ++i)
        out << "+" << (*i).c_str() << ",";

	string res = out.str();
    return res.substr(0, res.size() - 1);
}

void PlayerbotDbStore::Reset(PlayerbotAI *ai)
{
    uint64 guid = ai->GetBot()->GetObjectGuid().GetRawValue();
    uint32 account = sObjectMgr.GetPlayerAccountIdByGUID(ObjectGuid(guid));
    if (sPlayerbotAIConfig.IsInRandomAccountList(account))
        return;

    CharacterDatabase.PExecute("DELETE FROM `ai_playerbot_db_store` WHERE `guid` = '%u'", guid);
}

void PlayerbotDbStore::SaveValue(uint64 guid, string key, string value)
{
    CharacterDatabase.PExecute("INSERT INTO `ai_playerbot_db_store` (`guid`, `key`, `value`) VALUES ('%u', '%s', '%s')", guid, key.c_str(), value.c_str());
}
