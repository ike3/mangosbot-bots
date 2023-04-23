#include "botpch.h"
#include "../../playerbot.h"
#include "ClassStrategy.h"

using namespace ai;

#ifdef MANGOSBOT_ZERO // Vanilla

void ClassStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode(
        "invalid target",
        NextAction::array(0, new NextAction("select new target", 89.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "mounted",
        NextAction::array(0, new NextAction("check mount state", 88.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "very often",
        NextAction::array(0, new NextAction("use lightwell", 80.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "very often",
        NextAction::array(0, new NextAction("adamantite grenade", 61.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "often",
        NextAction::array(0, new NextAction("use trinket", 50.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "combat long stuck",
        NextAction::array(0, new NextAction("hearthstone", 0.9f), new NextAction("repop", 0.8f), NULL)));

    triggers.push_back(new TriggerNode(
        "combat stuck",
        NextAction::array(0, new NextAction("reset", 0.7f), NULL)));
}

void ClassStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{
    /*
    triggers.push_back(new TriggerNode(
        "vehicle near",
        NextAction::array(0, new NextAction("enter vehicle", 10.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "very often",
        NextAction::array(0, new NextAction("use lightwell", 80.0f), NULL)));
    */

    triggers.push_back(new TriggerNode(
        "very often",
        NextAction::array(0, new NextAction("check mount state", 1.0f), new NextAction("check values", 1.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "near dark portal",
        NextAction::array(0, new NextAction("move to dark portal", 1.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "at dark portal azeroth",
        NextAction::array(0, new NextAction("use dark portal azeroth", 1.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "at dark portal outland",
        NextAction::array(0, new NextAction("move from dark portal", 1.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "need world buff",
        NextAction::array(0, new NextAction("world buff", 1.0f), NULL)));
}

void ClassStrategy::InitReactionTriggers(std::list<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode(
        "combat start",
        NextAction::array(0, new NextAction("set combat state", 60.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "combat end",
        NextAction::array(0, new NextAction("set non combat state", 60.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "death",
        NextAction::array(0, new NextAction("set dead state", 60.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "resurrect",
        NextAction::array(0, new NextAction("set non combat state", 60.0f), NULL)));
}

void ClassStrategy::InitDeadTriggers(std::list<TriggerNode*>& triggers)
{

}

void ClassPvpStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{

}

void ClassPvpStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{

}

void ClassPvpStrategy::InitReactionTriggers(std::list<TriggerNode*>& triggers)
{

}

void ClassPvpStrategy::InitDeadTriggers(std::list<TriggerNode*>& triggers)
{

}

void ClassPveStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{

}

void ClassPveStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{

}

void ClassPveStrategy::InitReactionTriggers(std::list<TriggerNode*>& triggers)
{

}

void ClassPveStrategy::InitDeadTriggers(std::list<TriggerNode*>& triggers)
{

}

void ClassBossStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{

}

void ClassBossStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{

}

void ClassBossStrategy::InitReactionTriggers(std::list<TriggerNode*>& triggers)
{

}

void ClassBossStrategy::InitDeadTriggers(std::list<TriggerNode*>& triggers)
{

}

void AoeStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{

}

void AoeStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{

}

void AoePvpStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{

}

void AoePvpStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{

}

void AoePveStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{

}

void AoePveStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{

}

void CureStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{

}

void CureStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{

}

void CurePvpStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{

}

void CurePvpStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{

}

void CurePveStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{

}

void CurePveStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{

}

void BoostStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{

}

void BoostStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{

}

void BoostPvpStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{

}

void BoostPvpStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{

}

void BoostPveStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{

}

void BoostPveStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{

}

void CcStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{

}

void CcStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{

}

void CcPvpStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{

}

void CcPvpStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{

}

void CcPveStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{

}

void CcPveStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{

}

#elif MANGOSBOT_ONE // TBC

void ClassStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode(
        "invalid target",
        NextAction::array(0, new NextAction("select new target", 89.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "mounted",
        NextAction::array(0, new NextAction("check mount state", 88.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "very often",
        NextAction::array(0, new NextAction("use lightwell", 80.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "very often",
        NextAction::array(0, new NextAction("adamantite grenade", 61.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "often",
        NextAction::array(0, new NextAction("use trinket", 50.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "combat long stuck",
        NextAction::array(0, new NextAction("hearthstone", 0.9f), new NextAction("repop", 0.8f), NULL)));

    triggers.push_back(new TriggerNode(
        "combat stuck",
        NextAction::array(0, new NextAction("reset", 0.7f), NULL)));
}

void ClassStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{
    /*
    triggers.push_back(new TriggerNode(
        "vehicle near",
        NextAction::array(0, new NextAction("enter vehicle", 10.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "very often",
        NextAction::array(0, new NextAction("use lightwell", 80.0f), NULL)));
    */

    triggers.push_back(new TriggerNode(
        "very often",
        NextAction::array(0, new NextAction("check mount state", 1.0f), new NextAction("check values", 1.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "near dark portal",
        NextAction::array(0, new NextAction("move to dark portal", 1.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "at dark portal azeroth",
        NextAction::array(0, new NextAction("use dark portal azeroth", 1.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "at dark portal outland",
        NextAction::array(0, new NextAction("move from dark portal", 1.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "need world buff",
        NextAction::array(0, new NextAction("world buff", 1.0f), NULL)));
}

void ClassStrategy::InitReactionTriggers(std::list<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode(
        "combat start",
        NextAction::array(0, new NextAction("set combat state", 60.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "combat end",
        NextAction::array(0, new NextAction("set non combat state", 60.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "death",
        NextAction::array(0, new NextAction("set dead state", 60.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "resurrect",
        NextAction::array(0, new NextAction("set non combat state", 60.0f), NULL)));
}

void ClassStrategy::InitDeadTriggers(std::list<TriggerNode*>& triggers)
{

}

void ClassPvpStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{

}

void ClassPvpStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{

}

void ClassPvpStrategy::InitReactionTriggers(std::list<TriggerNode*>& triggers)
{

}

void ClassPvpStrategy::InitDeadTriggers(std::list<TriggerNode*>& triggers)
{

}

void ClassPveStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{

}

void ClassPveStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{

}

void ClassPveStrategy::InitReactionTriggers(std::list<TriggerNode*>& triggers)
{

}

void ClassPveStrategy::InitDeadTriggers(std::list<TriggerNode*>& triggers)
{

}

void ClassBossStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{

}

void ClassBossStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{

}

void ClassBossStrategy::InitReactionTriggers(std::list<TriggerNode*>& triggers)
{

}

void ClassBossStrategy::InitDeadTriggers(std::list<TriggerNode*>& triggers)
{

}

void AoeStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{

}

void AoeStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{

}

void AoePvpStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{

}

void AoePvpStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{

}

void AoePveStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{

}

void AoePveStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{

}

void CureStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{

}

void CureStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{

}

void CurePvpStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{

}

void CurePvpStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{

}

void CurePveStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{

}

void CurePveStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{

}

void BoostStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{

}

void BoostStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{

}

void BoostPvpStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{

}

void BoostPvpStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{

}

void BoostPveStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{

}

void BoostPveStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{

}

void CcStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{

}

void CcStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{

}

void CcPvpStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{

}

void CcPvpStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{

}

void CcPveStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{

}

void CcPveStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{

}

#elif MANGOSBOT_TWO // WOTLK

void ClassStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode(
        "invalid target",
        NextAction::array(0, new NextAction("select new target", 89.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "mounted",
        NextAction::array(0, new NextAction("check mount state", 88.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "very often",
        NextAction::array(0, new NextAction("use lightwell", 80.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "very often",
        NextAction::array(0, new NextAction("adamantite grenade", 61.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "often",
        NextAction::array(0, new NextAction("use trinket", 50.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "combat long stuck",
        NextAction::array(0, new NextAction("hearthstone", 0.9f), new NextAction("repop", 0.8f), NULL)));

    triggers.push_back(new TriggerNode(
        "combat stuck",
        NextAction::array(0, new NextAction("reset", 0.7f), NULL)));
}

void ClassStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{
    /*
    triggers.push_back(new TriggerNode(
        "vehicle near",
        NextAction::array(0, new NextAction("enter vehicle", 10.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "very often",
        NextAction::array(0, new NextAction("use lightwell", 80.0f), NULL)));
    */

    triggers.push_back(new TriggerNode(
        "very often",
        NextAction::array(0, new NextAction("check mount state", 1.0f), new NextAction("check values", 1.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "near dark portal",
        NextAction::array(0, new NextAction("move to dark portal", 1.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "at dark portal azeroth",
        NextAction::array(0, new NextAction("use dark portal azeroth", 1.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "at dark portal outland",
        NextAction::array(0, new NextAction("move from dark portal", 1.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "need world buff",
        NextAction::array(0, new NextAction("world buff", 1.0f), NULL)));
}

void ClassStrategy::InitReactionTriggers(std::list<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode(
        "combat start",
        NextAction::array(0, new NextAction("set combat state", 60.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "combat end",
        NextAction::array(0, new NextAction("set non combat state", 60.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "death",
        NextAction::array(0, new NextAction("set dead state", 60.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "resurrect",
        NextAction::array(0, new NextAction("set non combat state", 60.0f), NULL)));
}

void ClassStrategy::InitDeadTriggers(std::list<TriggerNode*>& triggers)
{

}

void ClassPvpStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{

}

void ClassPvpStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{

}

void ClassPvpStrategy::InitReactionTriggers(std::list<TriggerNode*>& triggers)
{

}

void ClassPvpStrategy::InitDeadTriggers(std::list<TriggerNode*>& triggers)
{

}

void ClassPveStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{

}

void ClassPveStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{

}

void ClassPveStrategy::InitReactionTriggers(std::list<TriggerNode*>& triggers)
{

}

void ClassPveStrategy::InitDeadTriggers(std::list<TriggerNode*>& triggers)
{

}

void ClassBossStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{

}

void ClassBossStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{

}

void ClassBossStrategy::InitReactionTriggers(std::list<TriggerNode*>& triggers)
{

}

void ClassBossStrategy::InitDeadTriggers(std::list<TriggerNode*>& triggers)
{

}

void AoeStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{

}

void AoeStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{

}

void AoePvpStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{

}

void AoePvpStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{

}

void AoePveStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{

}

void AoePveStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{

}

void CureStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{

}

void CureStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{

}

void CurePvpStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{

}

void CurePvpStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{

}

void CurePveStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{

}

void CurePveStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{

}

void BoostStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{

}

void BoostStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{

}

void BoostPvpStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{

}

void BoostPvpStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{

}

void BoostPveStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{

}

void BoostPveStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{

}

void CcStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{

}

void CcStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{

}

void CcPvpStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{

}

void CcPvpStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{

}

void CcPveStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{

}

void CcPveStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{

}

#endif

void SpecPlaceholderStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode(
        "in pve",
        NextAction::array(0, new NextAction("update pve strats", 100.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "in pvp",
        NextAction::array(0, new NextAction("update pvp strats", 100.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "in boss fight",
        NextAction::array(0, new NextAction("update boss strats", 100.0f), NULL)));
}

void SpecPlaceholderStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode(
        "in pve",
        NextAction::array(0, new NextAction("update pve strats", 100.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "in pvp",
        NextAction::array(0, new NextAction("update pvp strats", 100.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "in boss fight",
        NextAction::array(0, new NextAction("update boss strats", 100.0f), NULL)));
}

void AoeBossStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{

}

void AoeBossStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{

}

void CureBossStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{

}

void CureBossStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{

}

void BoostBossStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{

}

void BoostBossStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{

}

void CcBossStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{

}

void CcBossStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{

}
