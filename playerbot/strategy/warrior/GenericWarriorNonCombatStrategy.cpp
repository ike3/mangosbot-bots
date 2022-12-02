#include "botpch.h"
#include "../../playerbot.h"
#include "GenericWarriorNonCombatStrategy.h"

using namespace ai;

void GenericWarriorNonCombatStrategy::InitNonCombatTriggers(std::list<TriggerNode*> &triggers)
{
   NonCombatStrategy::InitNonCombatTriggers(triggers);

   triggers.push_back(new TriggerNode(
      "often",
      NextAction::array(0, new NextAction("apply stone", 1.0f), NULL)));

   triggers.push_back(new TriggerNode(
       "battle stance",
       NextAction::array(0, new NextAction("battle stance", 1.0f), NULL)));
}
