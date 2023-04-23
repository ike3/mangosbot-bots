#include "botpch.h"
#include "../../playerbot.h"
#include "ElementalShamanStrategy.h"

using namespace ai;

class ElementalShamanStrategyActionNodeFactory : public NamedObjectFactory<ActionNode>
{
public:
    ElementalShamanStrategyActionNodeFactory()
    {
        creators["totem of wrath"] = &totem_of_wrath;
        creators["magma totem"] = &magma_totem;
        creators["strength of earth totem"] = &strength_of_earth_totem;
        creators["wrath of air totem"] = &wrath_of_air_totem;
        creators["mana spring totem"] = &mana_spring_totem;
    }

private:
    ACTION_NODE_A(totem_of_wrath, "totem of wrath", "flametongue totem");

    ACTION_NODE_A(magma_totem, "magma totem", "fire nova");

    ACTION_NODE_A(strength_of_earth_totem, "strength of earth totem", "stoneskin totem");

    ACTION_NODE_A(wrath_of_air_totem, "wrath of air totem", "windfury totem");
   
    ACTION_NODE_A(mana_spring_totem, "mana spring totem", "healing stream totem");
};

ElementalShamanStrategy::ElementalShamanStrategy(PlayerbotAI* ai) 
: ShamanStrategy(ai)
{
    actionNodeFactories.Add(new ElementalShamanStrategyActionNodeFactory());
}

#ifdef MANGOSBOT_ZERO // Vanilla

NextAction** ElementalShamanStrategy::GetDefaultCombatActions()
{
    return NextAction::array(0, new NextAction("lightning bolt", 10.0f), NULL);
}

void ElementalShamanStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    ShamanStrategy::InitCombatTriggers(triggers);

    triggers.push_back(new TriggerNode(
        "shock",
        NextAction::array(0, new NextAction("earth shock", 20.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "frost shock snare",
        NextAction::array(0, new NextAction("frost shock", 21.0f), NULL)));
}

void ElementalShamanStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{
    ShamanStrategy::InitNonCombatTriggers(triggers);
}

void ElementalShamanStrategy::InitReactionTriggers(std::list<TriggerNode*>& triggers)
{
    ShamanStrategy::InitReactionTriggers(triggers);
}

void ElementalShamanStrategy::InitDeadTriggers(std::list<TriggerNode*>& triggers)
{
    ShamanStrategy::InitDeadTriggers(triggers);
}

void ElementalShamanPveStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    ElementalShamanStrategy::InitCombatTriggers(triggers);
    ShamanPveStrategy::InitCombatTriggers(triggers);
}

void ElementalShamanPveStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{
    ElementalShamanStrategy::InitNonCombatTriggers(triggers);
    ShamanPveStrategy::InitNonCombatTriggers(triggers);
}

void ElementalShamanPveStrategy::InitReactionTriggers(std::list<TriggerNode*>& triggers)
{
    ElementalShamanStrategy::InitReactionTriggers(triggers);
    ShamanPveStrategy::InitReactionTriggers(triggers);
}

void ElementalShamanPveStrategy::InitDeadTriggers(std::list<TriggerNode*>& triggers)
{
    ElementalShamanStrategy::InitDeadTriggers(triggers);
    ShamanPveStrategy::InitDeadTriggers(triggers);
}

void ElementalShamanPvpStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    ElementalShamanStrategy::InitCombatTriggers(triggers);
    ShamanPvpStrategy::InitCombatTriggers(triggers);
}

void ElementalShamanPvpStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{
    ElementalShamanStrategy::InitNonCombatTriggers(triggers);
    ShamanPvpStrategy::InitNonCombatTriggers(triggers);
}

void ElementalShamanPvpStrategy::InitReactionTriggers(std::list<TriggerNode*>& triggers)
{
    ElementalShamanStrategy::InitReactionTriggers(triggers);
    ShamanPvpStrategy::InitReactionTriggers(triggers);
}

void ElementalShamanPvpStrategy::InitDeadTriggers(std::list<TriggerNode*>& triggers)
{
    ElementalShamanStrategy::InitDeadTriggers(triggers);
    ShamanPvpStrategy::InitDeadTriggers(triggers);
}

void ElementalShamanBossStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    ElementalShamanStrategy::InitCombatTriggers(triggers);
    ShamanBossStrategy::InitCombatTriggers(triggers);
}

void ElementalShamanBossStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{
    ElementalShamanStrategy::InitNonCombatTriggers(triggers);
    ShamanBossStrategy::InitNonCombatTriggers(triggers);
}

void ElementalShamanBossStrategy::InitReactionTriggers(std::list<TriggerNode*>& triggers)
{
    ElementalShamanStrategy::InitReactionTriggers(triggers);
    ShamanBossStrategy::InitReactionTriggers(triggers);
}

void ElementalShamanBossStrategy::InitDeadTriggers(std::list<TriggerNode*>& triggers)
{
    ElementalShamanStrategy::InitDeadTriggers(triggers);
    ShamanBossStrategy::InitDeadTriggers(triggers);
}

void ElementalShamanAoeStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    ShamanAoeStrategy::InitCombatTriggers(triggers);

    triggers.push_back(new TriggerNode(
        "light aoe",
        NextAction::array(0, new NextAction("chain lightning", 25.0f), NULL)));
}

void ElementalShamanAoeStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{
    ShamanAoeStrategy::InitNonCombatTriggers(triggers);
}

void ElementalShamanAoePveStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    ElementalShamanAoeStrategy::InitCombatTriggers(triggers);
    ShamanAoePveStrategy::InitCombatTriggers(triggers);
}

void ElementalShamanAoePveStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{
    ElementalShamanAoeStrategy::InitNonCombatTriggers(triggers);
    ShamanAoePveStrategy::InitNonCombatTriggers(triggers);
}

void ElementalShamanAoePvpStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    ElementalShamanAoeStrategy::InitCombatTriggers(triggers);
    ShamanAoePvpStrategy::InitCombatTriggers(triggers);
}

void ElementalShamanAoePvpStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{
    ElementalShamanAoeStrategy::InitNonCombatTriggers(triggers);
    ShamanAoePvpStrategy::InitNonCombatTriggers(triggers);
}

void ElementalShamanAoeBossStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    ElementalShamanAoeStrategy::InitCombatTriggers(triggers);
    ShamanAoeBossStrategy::InitCombatTriggers(triggers);
}

void ElementalShamanAoeBossStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{
    ElementalShamanAoeStrategy::InitNonCombatTriggers(triggers);
    ShamanAoeBossStrategy::InitNonCombatTriggers(triggers);
}

void ElementalShamanCureStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    ShamanCureStrategy::InitCombatTriggers(triggers);
}

void ElementalShamanCureStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{
    ShamanCureStrategy::InitNonCombatTriggers(triggers);
}

void ElementalShamanCurePveStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    ElementalShamanCureStrategy::InitCombatTriggers(triggers);
    ShamanCurePveStrategy::InitCombatTriggers(triggers);
}

void ElementalShamanCurePveStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{
    ElementalShamanCureStrategy::InitNonCombatTriggers(triggers);
    ShamanCurePveStrategy::InitNonCombatTriggers(triggers);
}

void ElementalShamanCurePvpStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    ElementalShamanCureStrategy::InitCombatTriggers(triggers);
    ShamanCurePvpStrategy::InitCombatTriggers(triggers);
}

void ElementalShamanCurePvpStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{
    ElementalShamanCureStrategy::InitNonCombatTriggers(triggers);
    ShamanCurePvpStrategy::InitNonCombatTriggers(triggers);
}

void ElementalShamanCureBossStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    ElementalShamanCureStrategy::InitCombatTriggers(triggers);
    ShamanCureBossStrategy::InitCombatTriggers(triggers);
}

void ElementalShamanCureBossStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{
    ElementalShamanCureStrategy::InitNonCombatTriggers(triggers);
    ShamanCureBossStrategy::InitNonCombatTriggers(triggers);
}

void ElementalShamanTotemsStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    ShamanTotemsStrategy::InitCombatTriggers(triggers);

    triggers.push_back(new TriggerNode(
        "air totem",
        NextAction::array(0, new NextAction("wrath of air totem", 29.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "water totem",
        NextAction::array(0, new NextAction("mana spring totem", 28.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "earth totem",
        NextAction::array(0, new NextAction("strength of earth totem", 27.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "fire totem aoe",
        NextAction::array(0, new NextAction("magma totem", 26.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "fire totem",
        NextAction::array(0, new NextAction("totem of wrath", 25.0f), NULL)));
}

void ElementalShamanTotemsStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{
    ShamanTotemsStrategy::InitNonCombatTriggers(triggers);
}

void ElementalShamanTotemsPveStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    ElementalShamanTotemsStrategy::InitCombatTriggers(triggers);
    ShamanTotemsPveStrategy::InitCombatTriggers(triggers);
}

void ElementalShamanTotemsPveStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{
    ElementalShamanTotemsStrategy::InitNonCombatTriggers(triggers);
    ShamanTotemsPveStrategy::InitNonCombatTriggers(triggers);
}

void ElementalShamanTotemsPvpStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    ElementalShamanTotemsStrategy::InitCombatTriggers(triggers);
    ShamanTotemsPvpStrategy::InitCombatTriggers(triggers);
}

void ElementalShamanTotemsPvpStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{
    ElementalShamanTotemsStrategy::InitNonCombatTriggers(triggers);
    ShamanTotemsPvpStrategy::InitNonCombatTriggers(triggers);
}

void ElementalShamanTotemsBossStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    ElementalShamanTotemsStrategy::InitCombatTriggers(triggers);
    ShamanTotemsBossStrategy::InitCombatTriggers(triggers);
}

void ElementalShamanTotemsBossStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{
    ElementalShamanTotemsStrategy::InitNonCombatTriggers(triggers);
    ShamanTotemsBossStrategy::InitNonCombatTriggers(triggers);
}

#elif MANGOSBOT_ONE // TBC

NextAction** ElementalShamanStrategy::GetDefaultCombatActions()
{
    return NextAction::array(0, new NextAction("lightning bolt", 10.0f), NULL);
}

void ElementalShamanStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    ShamanStrategy::InitCombatTriggers(triggers);

    triggers.push_back(new TriggerNode(
        "shock",
        NextAction::array(0, new NextAction("earth shock", 20.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "frost shock snare",
        NextAction::array(0, new NextAction("frost shock", 21.0f), NULL)));
}

void ElementalShamanStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{
    ShamanStrategy::InitNonCombatTriggers(triggers);
}

void ElementalShamanStrategy::InitReactionTriggers(std::list<TriggerNode*>& triggers)
{
    ShamanStrategy::InitReactionTriggers(triggers);
}

void ElementalShamanStrategy::InitDeadTriggers(std::list<TriggerNode*>& triggers)
{
    ShamanStrategy::InitDeadTriggers(triggers);
}

void ElementalShamanPveStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    ElementalShamanStrategy::InitCombatTriggers(triggers);
    ShamanPveStrategy::InitCombatTriggers(triggers);
}

void ElementalShamanPveStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{
    ElementalShamanStrategy::InitNonCombatTriggers(triggers);
    ShamanPveStrategy::InitNonCombatTriggers(triggers);
}

void ElementalShamanPveStrategy::InitReactionTriggers(std::list<TriggerNode*>& triggers)
{
    ElementalShamanStrategy::InitReactionTriggers(triggers);
    ShamanPveStrategy::InitReactionTriggers(triggers);
}

void ElementalShamanPveStrategy::InitDeadTriggers(std::list<TriggerNode*>& triggers)
{
    ElementalShamanStrategy::InitDeadTriggers(triggers);
    ShamanPveStrategy::InitDeadTriggers(triggers);
}

void ElementalShamanPvpStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    ElementalShamanStrategy::InitCombatTriggers(triggers);
    ShamanPvpStrategy::InitCombatTriggers(triggers);
}

void ElementalShamanPvpStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{
    ElementalShamanStrategy::InitNonCombatTriggers(triggers);
    ShamanPvpStrategy::InitNonCombatTriggers(triggers);
}

void ElementalShamanPvpStrategy::InitReactionTriggers(std::list<TriggerNode*>& triggers)
{
    ElementalShamanStrategy::InitReactionTriggers(triggers);
    ShamanPvpStrategy::InitReactionTriggers(triggers);
}

void ElementalShamanPvpStrategy::InitDeadTriggers(std::list<TriggerNode*>& triggers)
{
    ElementalShamanStrategy::InitDeadTriggers(triggers);
    ShamanPvpStrategy::InitDeadTriggers(triggers);
}

void ElementalShamanBossStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    ElementalShamanStrategy::InitCombatTriggers(triggers);
    ShamanBossStrategy::InitCombatTriggers(triggers);
}

void ElementalShamanBossStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{
    ElementalShamanStrategy::InitNonCombatTriggers(triggers);
    ShamanBossStrategy::InitNonCombatTriggers(triggers);
}

void ElementalShamanBossStrategy::InitReactionTriggers(std::list<TriggerNode*>& triggers)
{
    ElementalShamanStrategy::InitReactionTriggers(triggers);
    ShamanBossStrategy::InitReactionTriggers(triggers);
}

void ElementalShamanBossStrategy::InitDeadTriggers(std::list<TriggerNode*>& triggers)
{
    ElementalShamanStrategy::InitDeadTriggers(triggers);
    ShamanBossStrategy::InitDeadTriggers(triggers);
}

void ElementalShamanAoeStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    ShamanAoeStrategy::InitCombatTriggers(triggers);

    triggers.push_back(new TriggerNode(
        "light aoe",
        NextAction::array(0, new NextAction("chain lightning", 25.0f), NULL)));
}

void ElementalShamanAoeStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{
    ShamanAoeStrategy::InitNonCombatTriggers(triggers);
}

void ElementalShamanAoePveStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    ElementalShamanAoeStrategy::InitCombatTriggers(triggers);
    ShamanAoePveStrategy::InitCombatTriggers(triggers);
}

void ElementalShamanAoePveStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{
    ElementalShamanAoeStrategy::InitNonCombatTriggers(triggers);
    ShamanAoePveStrategy::InitNonCombatTriggers(triggers);
}

void ElementalShamanAoePvpStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    ElementalShamanAoeStrategy::InitCombatTriggers(triggers);
    ShamanAoePvpStrategy::InitCombatTriggers(triggers);
}

void ElementalShamanAoePvpStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{
    ElementalShamanAoeStrategy::InitNonCombatTriggers(triggers);
    ShamanAoePvpStrategy::InitNonCombatTriggers(triggers);
}

void ElementalShamanAoeBossStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    ElementalShamanAoeStrategy::InitCombatTriggers(triggers);
    ShamanAoeBossStrategy::InitCombatTriggers(triggers);
}

void ElementalShamanAoeBossStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{
    ElementalShamanAoeStrategy::InitNonCombatTriggers(triggers);
    ShamanAoeBossStrategy::InitNonCombatTriggers(triggers);
}

void ElementalShamanCureStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    ShamanCureStrategy::InitCombatTriggers(triggers);
}

void ElementalShamanCureStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{
    ShamanCureStrategy::InitNonCombatTriggers(triggers);
}

void ElementalShamanCurePveStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    ElementalShamanCureStrategy::InitCombatTriggers(triggers);
    ShamanCurePveStrategy::InitCombatTriggers(triggers);
}

void ElementalShamanCurePveStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{
    ElementalShamanCureStrategy::InitNonCombatTriggers(triggers);
    ShamanCurePveStrategy::InitNonCombatTriggers(triggers);
}

void ElementalShamanCurePvpStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    ElementalShamanCureStrategy::InitCombatTriggers(triggers);
    ShamanCurePvpStrategy::InitCombatTriggers(triggers);
}

void ElementalShamanCurePvpStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{
    ElementalShamanCureStrategy::InitNonCombatTriggers(triggers);
    ShamanCurePvpStrategy::InitNonCombatTriggers(triggers);
}

void ElementalShamanCureBossStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    ElementalShamanCureStrategy::InitCombatTriggers(triggers);
    ShamanCureBossStrategy::InitCombatTriggers(triggers);
}

void ElementalShamanCureBossStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{
    ElementalShamanCureStrategy::InitNonCombatTriggers(triggers);
    ShamanCureBossStrategy::InitNonCombatTriggers(triggers);
}

void ElementalShamanTotemsStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    ShamanTotemsStrategy::InitCombatTriggers(triggers);

    triggers.push_back(new TriggerNode(
        "air totem",
        NextAction::array(0, new NextAction("wrath of air totem", 29.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "water totem",
        NextAction::array(0, new NextAction("mana spring totem", 28.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "earth totem",
        NextAction::array(0, new NextAction("strength of earth totem", 27.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "fire totem aoe",
        NextAction::array(0, new NextAction("magma totem", 26.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "fire totem",
        NextAction::array(0, new NextAction("totem of wrath", 25.0f), NULL)));
}

void ElementalShamanTotemsStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{
    ShamanTotemsStrategy::InitNonCombatTriggers(triggers);
}

void ElementalShamanTotemsPveStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    ElementalShamanTotemsStrategy::InitCombatTriggers(triggers);
    ShamanTotemsPveStrategy::InitCombatTriggers(triggers);
}

void ElementalShamanTotemsPveStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{
    ElementalShamanTotemsStrategy::InitNonCombatTriggers(triggers);
    ShamanTotemsPveStrategy::InitNonCombatTriggers(triggers);
}

void ElementalShamanTotemsPvpStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    ElementalShamanTotemsStrategy::InitCombatTriggers(triggers);
    ShamanTotemsPvpStrategy::InitCombatTriggers(triggers);
}

void ElementalShamanTotemsPvpStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{
    ElementalShamanTotemsStrategy::InitNonCombatTriggers(triggers);
    ShamanTotemsPvpStrategy::InitNonCombatTriggers(triggers);
}

void ElementalShamanTotemsBossStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    ElementalShamanTotemsStrategy::InitCombatTriggers(triggers);
    ShamanTotemsBossStrategy::InitCombatTriggers(triggers);
}

void ElementalShamanTotemsBossStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{
    ElementalShamanTotemsStrategy::InitNonCombatTriggers(triggers);
    ShamanTotemsBossStrategy::InitNonCombatTriggers(triggers);
}

#elif MANGOSBOT_TWO // WOTLK

NextAction** ElementalShamanStrategy::GetDefaultCombatActions()
{
    return NextAction::array(0, new NextAction("lightning bolt", 10.0f), NULL);
}

void ElementalShamanStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    ShamanStrategy::InitCombatTriggers(triggers);

    triggers.push_back(new TriggerNode(
        "shock",
        NextAction::array(0, new NextAction("earth shock", 20.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "frost shock snare",
        NextAction::array(0, new NextAction("frost shock", 21.0f), NULL)));
}

void ElementalShamanStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{
    ShamanStrategy::InitNonCombatTriggers(triggers);
}

void ElementalShamanStrategy::InitReactionTriggers(std::list<TriggerNode*>& triggers)
{
    ShamanStrategy::InitReactionTriggers(triggers);
}

void ElementalShamanStrategy::InitDeadTriggers(std::list<TriggerNode*>& triggers)
{
    ShamanStrategy::InitDeadTriggers(triggers);
}

void ElementalShamanPveStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    ElementalShamanStrategy::InitCombatTriggers(triggers);
    ShamanPveStrategy::InitCombatTriggers(triggers);
}

void ElementalShamanPveStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{
    ElementalShamanStrategy::InitNonCombatTriggers(triggers);
    ShamanPveStrategy::InitNonCombatTriggers(triggers);
}

void ElementalShamanPveStrategy::InitReactionTriggers(std::list<TriggerNode*>& triggers)
{
    ElementalShamanStrategy::InitReactionTriggers(triggers);
    ShamanPveStrategy::InitReactionTriggers(triggers);
}

void ElementalShamanPveStrategy::InitDeadTriggers(std::list<TriggerNode*>& triggers)
{
    ElementalShamanStrategy::InitDeadTriggers(triggers);
    ShamanPveStrategy::InitDeadTriggers(triggers);
}

void ElementalShamanPvpStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    ElementalShamanStrategy::InitCombatTriggers(triggers);
    ShamanPvpStrategy::InitCombatTriggers(triggers);
}

void ElementalShamanPvpStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{
    ElementalShamanStrategy::InitNonCombatTriggers(triggers);
    ShamanPvpStrategy::InitNonCombatTriggers(triggers);
}

void ElementalShamanPvpStrategy::InitReactionTriggers(std::list<TriggerNode*>& triggers)
{
    ElementalShamanStrategy::InitReactionTriggers(triggers);
    ShamanPvpStrategy::InitReactionTriggers(triggers);
}

void ElementalShamanPvpStrategy::InitDeadTriggers(std::list<TriggerNode*>& triggers)
{
    ElementalShamanStrategy::InitDeadTriggers(triggers);
    ShamanPvpStrategy::InitDeadTriggers(triggers);
}

void ElementalShamanBossStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    ElementalShamanStrategy::InitCombatTriggers(triggers);
    ShamanBossStrategy::InitCombatTriggers(triggers);
}

void ElementalShamanBossStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{
    ElementalShamanStrategy::InitNonCombatTriggers(triggers);
    ShamanBossStrategy::InitNonCombatTriggers(triggers);
}

void ElementalShamanBossStrategy::InitReactionTriggers(std::list<TriggerNode*>& triggers)
{
    ElementalShamanStrategy::InitReactionTriggers(triggers);
    ShamanBossStrategy::InitReactionTriggers(triggers);
}

void ElementalShamanBossStrategy::InitDeadTriggers(std::list<TriggerNode*>& triggers)
{
    ElementalShamanStrategy::InitDeadTriggers(triggers);
    ShamanBossStrategy::InitDeadTriggers(triggers);
}

void ElementalShamanAoeStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    ShamanAoeStrategy::InitCombatTriggers(triggers);

    triggers.push_back(new TriggerNode(
        "light aoe",
        NextAction::array(0, new NextAction("chain lightning", 25.0f), NULL)));
}

void ElementalShamanAoeStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{
    ShamanAoeStrategy::InitNonCombatTriggers(triggers);
}

void ElementalShamanAoePveStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    ElementalShamanAoeStrategy::InitCombatTriggers(triggers);
    ShamanAoePveStrategy::InitCombatTriggers(triggers);
}

void ElementalShamanAoePveStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{
    ElementalShamanAoeStrategy::InitNonCombatTriggers(triggers);
    ShamanAoePveStrategy::InitNonCombatTriggers(triggers);
}

void ElementalShamanAoePvpStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    ElementalShamanAoeStrategy::InitCombatTriggers(triggers);
    ShamanAoePvpStrategy::InitCombatTriggers(triggers);
}

void ElementalShamanAoePvpStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{
    ElementalShamanAoeStrategy::InitNonCombatTriggers(triggers);
    ShamanAoePvpStrategy::InitNonCombatTriggers(triggers);
}

void ElementalShamanAoeBossStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    ElementalShamanAoeStrategy::InitCombatTriggers(triggers);
    ShamanAoeBossStrategy::InitCombatTriggers(triggers);
}

void ElementalShamanAoeBossStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{
    ElementalShamanAoeStrategy::InitNonCombatTriggers(triggers);
    ShamanAoeBossStrategy::InitNonCombatTriggers(triggers);
}

void ElementalShamanCureStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    ShamanCureStrategy::InitCombatTriggers(triggers);
}

void ElementalShamanCureStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{
    ShamanCureStrategy::InitNonCombatTriggers(triggers);
}

void ElementalShamanCurePveStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    ElementalShamanCureStrategy::InitCombatTriggers(triggers);
    ShamanCurePveStrategy::InitCombatTriggers(triggers);
}

void ElementalShamanCurePveStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{
    ElementalShamanCureStrategy::InitNonCombatTriggers(triggers);
    ShamanCurePveStrategy::InitNonCombatTriggers(triggers);
}

void ElementalShamanCurePvpStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    ElementalShamanCureStrategy::InitCombatTriggers(triggers);
    ShamanCurePvpStrategy::InitCombatTriggers(triggers);
}

void ElementalShamanCurePvpStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{
    ElementalShamanCureStrategy::InitNonCombatTriggers(triggers);
    ShamanCurePvpStrategy::InitNonCombatTriggers(triggers);
}

void ElementalShamanCureBossStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    ElementalShamanCureStrategy::InitCombatTriggers(triggers);
    ShamanCureBossStrategy::InitCombatTriggers(triggers);
}

void ElementalShamanCureBossStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{
    ElementalShamanCureStrategy::InitNonCombatTriggers(triggers);
    ShamanCureBossStrategy::InitNonCombatTriggers(triggers);
}

void ElementalShamanTotemsStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    ShamanTotemsStrategy::InitCombatTriggers(triggers);

    triggers.push_back(new TriggerNode(
        "air totem",
        NextAction::array(0, new NextAction("wrath of air totem", 29.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "water totem",
        NextAction::array(0, new NextAction("mana spring totem", 28.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "earth totem",
        NextAction::array(0, new NextAction("strength of earth totem", 27.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "fire totem aoe",
        NextAction::array(0, new NextAction("magma totem", 26.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "fire totem",
        NextAction::array(0, new NextAction("totem of wrath", 25.0f), NULL)));
}

void ElementalShamanTotemsStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{
    ShamanTotemsStrategy::InitNonCombatTriggers(triggers);
}

void ElementalShamanTotemsPveStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    ElementalShamanTotemsStrategy::InitCombatTriggers(triggers);
    ShamanTotemsPveStrategy::InitCombatTriggers(triggers);
}

void ElementalShamanTotemsPveStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{
    ElementalShamanTotemsStrategy::InitNonCombatTriggers(triggers);
    ShamanTotemsPveStrategy::InitNonCombatTriggers(triggers);
}

void ElementalShamanTotemsPvpStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    ElementalShamanTotemsStrategy::InitCombatTriggers(triggers);
    ShamanTotemsPvpStrategy::InitCombatTriggers(triggers);
}

void ElementalShamanTotemsPvpStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{
    ElementalShamanTotemsStrategy::InitNonCombatTriggers(triggers);
    ShamanTotemsPvpStrategy::InitNonCombatTriggers(triggers);
}

void ElementalShamanTotemsBossStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    ElementalShamanTotemsStrategy::InitCombatTriggers(triggers);
    ShamanTotemsBossStrategy::InitCombatTriggers(triggers);
}

void ElementalShamanTotemsBossStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{
    ElementalShamanTotemsStrategy::InitNonCombatTriggers(triggers);
    ShamanTotemsBossStrategy::InitNonCombatTriggers(triggers);
}

#endif