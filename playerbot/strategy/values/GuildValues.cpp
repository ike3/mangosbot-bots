#include "botpch.h"
#include "../../playerbot.h"
#include "GuildValues.h"

using namespace ai;


uint8 PetitionSignsValue::Calculate()
{
    if (bot->GetGuildId())
       return 0;

    Item* petition = bot->GetItemByEntry(5863);

    if (!petition)
        return 0;

    QueryResult* result = CharacterDatabase.PQuery("SELECT playerguid FROM petition_sign WHERE petitionguid = '%u'", petition->GetObjectGuid().GetCounter());

    return result ? (uint8)result->GetRowCount() : 0;
};
