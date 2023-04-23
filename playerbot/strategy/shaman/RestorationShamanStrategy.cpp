#include "botpch.h"
#include "../../playerbot.h"
#include "ShamanMultipliers.h"
#include "RestorationShamanStrategy.h"

using namespace ai;

class RestorationShamanStrategyActionNodeFactory : public NamedObjectFactory<ActionNode>
{
public:
    RestorationShamanStrategyActionNodeFactory()
    {
        creators["earthliving weapon"] = &earthliving_weapon;
        creators["mana tide totem"] = &mana_tide_totem;
        creators["flametongue totem"] = &flametongue_totem;
        creators["magma totem"] = &magma_totem;
        creators["strength of earth totem"] = &strength_of_earth_totem;
        creators["wrath of air totem"] = &wrath_of_air_totem;
        creators["healing stream totem"] = &healing_stream_totem;
    }

private:
    ACTION_NODE_A(earthliving_weapon, "earthliving weapon", "flametongue weapon");

    ACTION_NODE_A(mana_tide_totem, "mana tide totem", "mana potion");

    ACTION_NODE_A(flametongue_totem, "flametongue totem", "searing totem");

    ACTION_NODE_C(magma_totem, "magma totem", "fire nova");

    ACTION_NODE_A(strength_of_earth_totem, "strength of earth totem", "stoneskin totem");

    ACTION_NODE_A(wrath_of_air_totem, "wrath of air totem", "windfury totem");

    ACTION_NODE_A(healing_stream_totem, "healing stream totem", "mana spring totem");
};

RestorationShamanStrategy::RestorationShamanStrategy(PlayerbotAI* ai) : ShamanStrategy(ai)
{
    actionNodeFactories.Add(new RestorationShamanStrategyActionNodeFactory());
}

#ifdef MANGOSBOT_ZERO // Vanilla

void RestorationShamanStrategy::InitCombatTriggers(std::list<TriggerNode*> &triggers)
{
    ShamanStrategy::InitCombatTriggers(triggers);

    triggers.push_back(new TriggerNode(
        "shaman weapon",
        NextAction::array(0, new NextAction("earthliving weapon", 22.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "low mana",
        NextAction::array(0, new NextAction("mana tide totem", ACTION_EMERGENCY + 5), NULL)));
}

void RestorationShamanStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{
    ShamanStrategy::InitNonCombatTriggers(triggers);
}

void RestorationShamanStrategy::InitReactionTriggers(std::list<TriggerNode*>& triggers)
{
    ShamanStrategy::InitReactionTriggers(triggers);
}

void RestorationShamanStrategy::InitDeadTriggers(std::list<TriggerNode*>& triggers)
{
    ShamanStrategy::InitDeadTriggers(triggers);
}

void RestorationShamanPveStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    RestorationShamanStrategy::InitCombatTriggers(triggers);
    ShamanPveStrategy::InitCombatTriggers(triggers);
}

void RestorationShamanPveStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{
    RestorationShamanStrategy::InitNonCombatTriggers(triggers);
    ShamanPveStrategy::InitNonCombatTriggers(triggers);
}

void RestorationShamanPveStrategy::InitReactionTriggers(std::list<TriggerNode*>& triggers)
{
    RestorationShamanStrategy::InitReactionTriggers(triggers);
    ShamanPveStrategy::InitReactionTriggers(triggers);
}

void RestorationShamanPveStrategy::InitDeadTriggers(std::list<TriggerNode*>& triggers)
{
    RestorationShamanStrategy::InitDeadTriggers(triggers);
    ShamanPveStrategy::InitDeadTriggers(triggers);
}

void RestorationShamanPvpStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    RestorationShamanStrategy::InitCombatTriggers(triggers);
    ShamanPvpStrategy::InitCombatTriggers(triggers);
}

void RestorationShamanPvpStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{
    RestorationShamanStrategy::InitNonCombatTriggers(triggers);
    ShamanPvpStrategy::InitNonCombatTriggers(triggers);
}

void RestorationShamanPvpStrategy::InitReactionTriggers(std::list<TriggerNode*>& triggers)
{
    RestorationShamanStrategy::InitReactionTriggers(triggers);
    ShamanPvpStrategy::InitReactionTriggers(triggers);
}

void RestorationShamanPvpStrategy::InitDeadTriggers(std::list<TriggerNode*>& triggers)
{
    RestorationShamanStrategy::InitDeadTriggers(triggers);
    ShamanPvpStrategy::InitDeadTriggers(triggers);
}

void RestorationShamanAoeStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    ShamanAoeStrategy::InitCombatTriggers(triggers);
}

void RestorationShamanAoeStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{
    ShamanAoeStrategy::InitNonCombatTriggers(triggers);
}

void RestorationShamanAoePveStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    RestorationShamanAoeStrategy::InitCombatTriggers(triggers);
    ShamanAoePveStrategy::InitCombatTriggers(triggers);
}

void RestorationShamanAoePveStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{
    RestorationShamanAoeStrategy::InitNonCombatTriggers(triggers);
    ShamanAoePveStrategy::InitNonCombatTriggers(triggers);
}

void RestorationShamanAoePvpStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    RestorationShamanAoeStrategy::InitCombatTriggers(triggers);
    ShamanAoePvpStrategy::InitCombatTriggers(triggers);
}

void RestorationShamanAoePvpStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{
    RestorationShamanAoeStrategy::InitNonCombatTriggers(triggers);
    ShamanAoePvpStrategy::InitNonCombatTriggers(triggers);
}

void RestorationShamanCureStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    ShamanCureStrategy::InitCombatTriggers(triggers);
}

void RestorationShamanCureStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{
    ShamanCureStrategy::InitNonCombatTriggers(triggers);
}

void RestorationShamanCurePveStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    RestorationShamanCureStrategy::InitCombatTriggers(triggers);
    ShamanCurePveStrategy::InitCombatTriggers(triggers);
}

void RestorationShamanCurePveStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{
    RestorationShamanCureStrategy::InitNonCombatTriggers(triggers);
    ShamanCurePveStrategy::InitNonCombatTriggers(triggers);
}

void RestorationShamanCurePvpStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    RestorationShamanCureStrategy::InitCombatTriggers(triggers);
    ShamanCurePvpStrategy::InitCombatTriggers(triggers);
}

void RestorationShamanCurePvpStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{
    RestorationShamanCureStrategy::InitNonCombatTriggers(triggers);
    ShamanCurePvpStrategy::InitNonCombatTriggers(triggers);
}

void RestorationShamanTotemsStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    ShamanTotemsStrategy::InitCombatTriggers(triggers);

    triggers.push_back(new TriggerNode(
        "air totem",
        NextAction::array(0, new NextAction("wrath of air totem", 29.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "water totem",
        NextAction::array(0, new NextAction("healing stream totem", 28.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "earth totem",
        NextAction::array(0, new NextAction("strength of earth totem", 27.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "fire totem aoe",
        NextAction::array(0, new NextAction("magma totem", 26.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "fire totem",
        NextAction::array(0, new NextAction("flametongue totem", 25.0f), NULL)));
}

void RestorationShamanTotemsStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{
    ShamanTotemsStrategy::InitNonCombatTriggers(triggers);
}

void RestorationShamanTotemsPveStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    RestorationShamanTotemsStrategy::InitCombatTriggers(triggers);
    ShamanTotemsPveStrategy::InitCombatTriggers(triggers);
}

void RestorationShamanTotemsPveStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{
    RestorationShamanTotemsStrategy::InitNonCombatTriggers(triggers);
    ShamanTotemsPveStrategy::InitNonCombatTriggers(triggers);
}

void RestorationShamanTotemsPvpStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    RestorationShamanTotemsStrategy::InitCombatTriggers(triggers);
    ShamanTotemsPvpStrategy::InitCombatTriggers(triggers);
}

void RestorationShamanTotemsPvpStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{
    RestorationShamanTotemsStrategy::InitNonCombatTriggers(triggers);
    ShamanTotemsPvpStrategy::InitNonCombatTriggers(triggers);
}

#elif MANGOSBOT_ONE // TBC

void RestorationShamanStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    ShamanStrategy::InitCombatTriggers(triggers);

    triggers.push_back(new TriggerNode(
        "shaman weapon",
        NextAction::array(0, new NextAction("earthliving weapon", 22.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "low mana",
        NextAction::array(0, new NextAction("mana tide totem", ACTION_EMERGENCY + 5), NULL)));
}

void RestorationShamanStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{
    ShamanStrategy::InitNonCombatTriggers(triggers);
}

void RestorationShamanStrategy::InitReactionTriggers(std::list<TriggerNode*>& triggers)
{
    ShamanStrategy::InitReactionTriggers(triggers);
}

void RestorationShamanStrategy::InitDeadTriggers(std::list<TriggerNode*>& triggers)
{
    ShamanStrategy::InitDeadTriggers(triggers);
}

void RestorationShamanPveStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    RestorationShamanStrategy::InitCombatTriggers(triggers);
    ShamanPveStrategy::InitCombatTriggers(triggers);
}

void RestorationShamanPveStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{
    RestorationShamanStrategy::InitNonCombatTriggers(triggers);
    ShamanPveStrategy::InitNonCombatTriggers(triggers);
}

void RestorationShamanPveStrategy::InitReactionTriggers(std::list<TriggerNode*>& triggers)
{
    RestorationShamanStrategy::InitReactionTriggers(triggers);
    ShamanPveStrategy::InitReactionTriggers(triggers);
}

void RestorationShamanPveStrategy::InitDeadTriggers(std::list<TriggerNode*>& triggers)
{
    RestorationShamanStrategy::InitDeadTriggers(triggers);
    ShamanPveStrategy::InitDeadTriggers(triggers);
}

void RestorationShamanPvpStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    RestorationShamanStrategy::InitCombatTriggers(triggers);
    ShamanPvpStrategy::InitCombatTriggers(triggers);
}

void RestorationShamanPvpStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{
    RestorationShamanStrategy::InitNonCombatTriggers(triggers);
    ShamanPvpStrategy::InitNonCombatTriggers(triggers);
}

void RestorationShamanPvpStrategy::InitReactionTriggers(std::list<TriggerNode*>& triggers)
{
    RestorationShamanStrategy::InitReactionTriggers(triggers);
    ShamanPvpStrategy::InitReactionTriggers(triggers);
}

void RestorationShamanPvpStrategy::InitDeadTriggers(std::list<TriggerNode*>& triggers)
{
    RestorationShamanStrategy::InitDeadTriggers(triggers);
    ShamanPvpStrategy::InitDeadTriggers(triggers);
}

void RestorationShamanAoeStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    ShamanAoeStrategy::InitCombatTriggers(triggers);
}

void RestorationShamanAoeStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{
    ShamanAoeStrategy::InitNonCombatTriggers(triggers);
}

void RestorationShamanAoePveStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    RestorationShamanAoeStrategy::InitCombatTriggers(triggers);
    ShamanAoePveStrategy::InitCombatTriggers(triggers);
}

void RestorationShamanAoePveStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{
    RestorationShamanAoeStrategy::InitNonCombatTriggers(triggers);
    ShamanAoePveStrategy::InitNonCombatTriggers(triggers);
}

void RestorationShamanAoePvpStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    RestorationShamanAoeStrategy::InitCombatTriggers(triggers);
    ShamanAoePvpStrategy::InitCombatTriggers(triggers);
}

void RestorationShamanAoePvpStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{
    RestorationShamanAoeStrategy::InitNonCombatTriggers(triggers);
    ShamanAoePvpStrategy::InitNonCombatTriggers(triggers);
}

void RestorationShamanCureStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    ShamanCureStrategy::InitCombatTriggers(triggers);
}

void RestorationShamanCureStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{
    ShamanCureStrategy::InitNonCombatTriggers(triggers);
}

void RestorationShamanCurePveStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    RestorationShamanCureStrategy::InitCombatTriggers(triggers);
    ShamanCurePveStrategy::InitCombatTriggers(triggers);
}

void RestorationShamanCurePveStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{
    RestorationShamanCureStrategy::InitNonCombatTriggers(triggers);
    ShamanCurePveStrategy::InitNonCombatTriggers(triggers);
}

void RestorationShamanCurePvpStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    RestorationShamanCureStrategy::InitCombatTriggers(triggers);
    ShamanCurePvpStrategy::InitCombatTriggers(triggers);
}

void RestorationShamanCurePvpStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{
    RestorationShamanCureStrategy::InitNonCombatTriggers(triggers);
    ShamanCurePvpStrategy::InitNonCombatTriggers(triggers);
}

void RestorationShamanTotemsStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    ShamanTotemsStrategy::InitCombatTriggers(triggers);

    triggers.push_back(new TriggerNode(
        "air totem",
        NextAction::array(0, new NextAction("wrath of air totem", 29.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "water totem",
        NextAction::array(0, new NextAction("healing stream totem", 28.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "earth totem",
        NextAction::array(0, new NextAction("strength of earth totem", 27.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "fire totem aoe",
        NextAction::array(0, new NextAction("magma totem", 26.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "fire totem",
        NextAction::array(0, new NextAction("flametongue totem", 25.0f), NULL)));
}

void RestorationShamanTotemsStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{
    ShamanTotemsStrategy::InitNonCombatTriggers(triggers);
}

void RestorationShamanTotemsPveStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    RestorationShamanTotemsStrategy::InitCombatTriggers(triggers);
    ShamanTotemsPveStrategy::InitCombatTriggers(triggers);
}

void RestorationShamanTotemsPveStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{
    RestorationShamanTotemsStrategy::InitNonCombatTriggers(triggers);
    ShamanTotemsPveStrategy::InitNonCombatTriggers(triggers);
}

void RestorationShamanTotemsPvpStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    RestorationShamanTotemsStrategy::InitCombatTriggers(triggers);
    ShamanTotemsPvpStrategy::InitCombatTriggers(triggers);
}

void RestorationShamanTotemsPvpStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{
    RestorationShamanTotemsStrategy::InitNonCombatTriggers(triggers);
    ShamanTotemsPvpStrategy::InitNonCombatTriggers(triggers);
}

#elif MANGOSBOT_TWO // WOTLK

void RestorationShamanStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    ShamanStrategy::InitCombatTriggers(triggers);

    triggers.push_back(new TriggerNode(
        "shaman weapon",
        NextAction::array(0, new NextAction("earthliving weapon", 22.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "low mana",
        NextAction::array(0, new NextAction("mana tide totem", ACTION_EMERGENCY + 5), NULL)));
}

void RestorationShamanStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{
    ShamanStrategy::InitNonCombatTriggers(triggers);
}

void RestorationShamanStrategy::InitReactionTriggers(std::list<TriggerNode*>& triggers)
{
    ShamanStrategy::InitReactionTriggers(triggers);
}

void RestorationShamanStrategy::InitDeadTriggers(std::list<TriggerNode*>& triggers)
{
    ShamanStrategy::InitDeadTriggers(triggers);
}

void RestorationShamanPveStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    RestorationShamanStrategy::InitCombatTriggers(triggers);
    ShamanPveStrategy::InitCombatTriggers(triggers);
}

void RestorationShamanPveStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{
    RestorationShamanStrategy::InitNonCombatTriggers(triggers);
    ShamanPveStrategy::InitNonCombatTriggers(triggers);
}

void RestorationShamanPveStrategy::InitReactionTriggers(std::list<TriggerNode*>& triggers)
{
    RestorationShamanStrategy::InitReactionTriggers(triggers);
    ShamanPveStrategy::InitReactionTriggers(triggers);
}

void RestorationShamanPveStrategy::InitDeadTriggers(std::list<TriggerNode*>& triggers)
{
    RestorationShamanStrategy::InitDeadTriggers(triggers);
    ShamanPveStrategy::InitDeadTriggers(triggers);
}

void RestorationShamanPvpStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    RestorationShamanStrategy::InitCombatTriggers(triggers);
    ShamanPvpStrategy::InitCombatTriggers(triggers);
}

void RestorationShamanPvpStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{
    RestorationShamanStrategy::InitNonCombatTriggers(triggers);
    ShamanPvpStrategy::InitNonCombatTriggers(triggers);
}

void RestorationShamanPvpStrategy::InitReactionTriggers(std::list<TriggerNode*>& triggers)
{
    RestorationShamanStrategy::InitReactionTriggers(triggers);
    ShamanPvpStrategy::InitReactionTriggers(triggers);
}

void RestorationShamanPvpStrategy::InitDeadTriggers(std::list<TriggerNode*>& triggers)
{
    RestorationShamanStrategy::InitDeadTriggers(triggers);
    ShamanPvpStrategy::InitDeadTriggers(triggers);
}

void RestorationShamanAoeStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    ShamanAoeStrategy::InitCombatTriggers(triggers);
}

void RestorationShamanAoeStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{
    ShamanAoeStrategy::InitNonCombatTriggers(triggers);
}

void RestorationShamanAoePveStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    RestorationShamanAoeStrategy::InitCombatTriggers(triggers);
    ShamanAoePveStrategy::InitCombatTriggers(triggers);
}

void RestorationShamanAoePveStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{
    RestorationShamanAoeStrategy::InitNonCombatTriggers(triggers);
    ShamanAoePveStrategy::InitNonCombatTriggers(triggers);
}

void RestorationShamanAoePvpStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    RestorationShamanAoeStrategy::InitCombatTriggers(triggers);
    ShamanAoePvpStrategy::InitCombatTriggers(triggers);
}

void RestorationShamanAoePvpStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{
    RestorationShamanAoeStrategy::InitNonCombatTriggers(triggers);
    ShamanAoePvpStrategy::InitNonCombatTriggers(triggers);
}

void RestorationShamanCureStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    ShamanCureStrategy::InitCombatTriggers(triggers);
}

void RestorationShamanCureStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{
    ShamanCureStrategy::InitNonCombatTriggers(triggers);
}

void RestorationShamanCurePveStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    RestorationShamanCureStrategy::InitCombatTriggers(triggers);
    ShamanCurePveStrategy::InitCombatTriggers(triggers);
}

void RestorationShamanCurePveStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{
    RestorationShamanCureStrategy::InitNonCombatTriggers(triggers);
    ShamanCurePveStrategy::InitNonCombatTriggers(triggers);
}

void RestorationShamanCurePvpStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    RestorationShamanCureStrategy::InitCombatTriggers(triggers);
    ShamanCurePvpStrategy::InitCombatTriggers(triggers);
}

void RestorationShamanCurePvpStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{
    RestorationShamanCureStrategy::InitNonCombatTriggers(triggers);
    ShamanCurePvpStrategy::InitNonCombatTriggers(triggers);
}

void RestorationShamanTotemsStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    ShamanTotemsStrategy::InitCombatTriggers(triggers);

    triggers.push_back(new TriggerNode(
        "air totem",
        NextAction::array(0, new NextAction("wrath of air totem", 29.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "water totem",
        NextAction::array(0, new NextAction("healing stream totem", 28.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "earth totem",
        NextAction::array(0, new NextAction("strength of earth totem", 27.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "fire totem aoe",
        NextAction::array(0, new NextAction("magma totem", 26.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "fire totem",
        NextAction::array(0, new NextAction("flametongue totem", 25.0f), NULL)));
}

void RestorationShamanTotemsStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{
    ShamanTotemsStrategy::InitNonCombatTriggers(triggers);
}

void RestorationShamanTotemsPveStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    RestorationShamanTotemsStrategy::InitCombatTriggers(triggers);
    ShamanTotemsPveStrategy::InitCombatTriggers(triggers);
}

void RestorationShamanTotemsPveStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{
    RestorationShamanTotemsStrategy::InitNonCombatTriggers(triggers);
    ShamanTotemsPveStrategy::InitNonCombatTriggers(triggers);
}

void RestorationShamanTotemsPvpStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    RestorationShamanTotemsStrategy::InitCombatTriggers(triggers);
    ShamanTotemsPvpStrategy::InitCombatTriggers(triggers);
}

void RestorationShamanTotemsPvpStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{
    RestorationShamanTotemsStrategy::InitNonCombatTriggers(triggers);
    ShamanTotemsPvpStrategy::InitNonCombatTriggers(triggers);
}

#endif