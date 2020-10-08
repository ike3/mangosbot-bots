#include "botpch.h"
#include "../../playerbot.h"
#include "RogueTriggers.h"
#include "RogueMultipliers.h"
#include "GenericRogueNonCombatStrategy.h"
#include "RogueActions.h"

using namespace ai;

void GenericRogueNonCombatStrategy::InitTriggers(std::list<TriggerNode*> &triggers)
{
	//TODO Possible crash on CMSG_USE_ITEM ByteBuffer error
    NonCombatStrategy::InitTriggers(triggers);
    
    triggers.push_back(new TriggerNode(
       "often",
       NextAction::array(0, new NextAction("apply poison", 1.0f), NULL)));

       /*triggers.push_back(new TriggerNode(
        "player has no flag",
        NextAction::array(0, new NextAction("stealth", ACTION_HIGH), NULL)));*/

    /*triggers.push_back(new TriggerNode(
        "enemy out of melee",
        NextAction::array(0, new NextAction("stealth", ACTION_HIGH + 1), NULL)));*/

    triggers.push_back(new TriggerNode(
        "player has flag",
        NextAction::array(0, new NextAction("sprint", ACTION_EMERGENCY + 1), NULL)));

    triggers.push_back(new TriggerNode(
        "enemy flagcarrier near",
        NextAction::array(0, new NextAction("sprint", ACTION_EMERGENCY + 2), NULL)));

}
