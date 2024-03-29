#include "botpch.h"
#include "../../playerbot.h"
#include "PriestMultipliers.h"
#include "HealPriestStrategy.h"
#include "GenericPriestStrategyActionNodeFactory.h"

using namespace ai;

GenericPriestStrategy::GenericPriestStrategy(PlayerbotAI* ai) : CombatStrategy(ai)
{
    actionNodeFactories.Add(new GenericPriestStrategyActionNodeFactory());
}

void GenericPriestStrategy::InitTriggers(std::list<TriggerNode*> &triggers)
{
    CombatStrategy::InitTriggers(triggers);

    triggers.push_back(new TriggerNode(
        "critical health",
        NextAction::array(0, new NextAction("power word: shield", 70.0f), new NextAction("flash heal", 70.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "party member critical health",
        NextAction::array(0, new NextAction("power word: shield on party", 60.0f), new NextAction("flash heal on party", 60.0f), NULL)));


    triggers.push_back(new TriggerNode(
        "low health",
        NextAction::array(0, new NextAction("power word: shield", 60.0f), new NextAction("greater heal", 60.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "party member low health",
        NextAction::array(0, new NextAction("power word: shield on party", 50.0f), new NextAction("greater heal on party", 50.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "medium threat",
        NextAction::array(0, new NextAction("psychic scream", 50.0f), NULL)));

}

void PriestCureStrategy::InitTriggers(std::list<TriggerNode*> &triggers)
{
    triggers.push_back(new TriggerNode(
        "dispel magic",
        NextAction::array(0, new NextAction("dispel magic", 41.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "dispel magic on party",
        NextAction::array(0, new NextAction("dispel magic on party", 40.0f), NULL)));


    triggers.push_back(new TriggerNode(
        "cure disease",
        NextAction::array(0, new NextAction("abolish disease", 31.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "party member cure disease",
        NextAction::array(0, new NextAction("abolish disease on party", 30.0f), NULL)));
}


void PriestBoostStrategy::InitTriggers(std::list<TriggerNode*> &triggers)
{
    triggers.push_back(new TriggerNode(
        "inner focus",
        NextAction::array(0, new NextAction("inner focus", 42.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "power infusion",
        NextAction::array(0, new NextAction("power infusion", 41.0f), NULL)));
}

void PriestCcStrategy::InitTriggers(std::list<TriggerNode*> &triggers)
{
    triggers.push_back(new TriggerNode(
        "shackle undead",
        NextAction::array(0, new NextAction("shackle undead", 31.0f), NULL)));
}