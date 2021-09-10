#include "botpch.h"
#include "../../playerbot.h"
#include "GroupValues.h"
#include "../../ServerFacade.h"

using namespace ai;

bool IsFollowingPartyValue::Calculate()
{
    if (ai->GetGroupMaster() == bot)
        return true;

    if (ai->HasStrategy("follow", BOT_STATE_NON_COMBAT))
        return true;

    return false;
};

bool IsNearLeaderValue::Calculate()
{
    Player* groupMaster = ai->GetGroupMaster();

    if (groupMaster == bot)
        return true;

    if (!groupMaster)
        return false;

    return sServerFacade.GetDistance2d(bot, ai->GetGroupMaster()) < sPlayerbotAIConfig.reactDistance;
}

bool BoolANDValue::Calculate()
{
    vector<string> values = split(getQualifier(), ',');

    for (auto value : values)
    {
        if(!AI_VALUE(bool, value))
            return false;
    }

    return true;
}

bool GroupBoolANDValue::Calculate()
{    
    Group* group = bot->GetGroup();
    if (group)
    {
        for (GroupReference* ref = group->GetFirstMember(); ref; ref = ref->next())
        {            
            Player* member = ref->getSource();

            PlayerbotAI* memberAi = member->GetPlayerbotAI();

            if (!memberAi)
                continue;

            AiObjectContext* memberContext = memberAi->GetAiObjectContext();

            if (!memberContext)
                continue;

            if (!memberContext->GetValue<bool>("and", getQualifier())->Get())
                return false;
        }

        return true;
    }
    else
        return AI_VALUE2(bool, "and", getQualifier());
};

bool GroupBoolORValue::Calculate()
{
    Group* group = bot->GetGroup();
    if (group)
    {
        for (GroupReference* ref = group->GetFirstMember(); ref; ref = ref->next())
        {
            Player* member = ref->getSource();

            PlayerbotAI* memberAi = member->GetPlayerbotAI();

            if (!memberAi)
                continue;

            AiObjectContext* memberContext = memberAi->GetAiObjectContext();

            if (!memberContext)
                continue;

            if (memberContext->GetValue<bool>("and", getQualifier())->Get())
                return true;
        }

        return false;
    }
    else
        return AI_VALUE2(bool, "and", getQualifier());
};