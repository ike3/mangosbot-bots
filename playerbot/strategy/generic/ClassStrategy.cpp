#include "botpch.h"
#include "../../playerbot.h"
#include "ClassStrategy.h"

using namespace ai;

#ifdef MANGOSBOT_ZERO // Vanilla

void ClassStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode(
        "invalid target",
        NextAction::array(0, new NextAction("select new target", ACTION_EMERGENCY), NULL)));

    triggers.push_back(new TriggerNode(
        "mounted",
        NextAction::array(0, new NextAction("check mount state", ACTION_EMERGENCY), NULL)));

    triggers.push_back(new TriggerNode(
        "very often",
        NextAction::array(0, new NextAction("use lightwell", ACTION_LIGHT_HEAL), NULL)));

    triggers.push_back(new TriggerNode(
        "combat long stuck",
        NextAction::array(0, new NextAction("hearthstone", ACTION_IDLE - 0.1f), new NextAction("repop", ACTION_IDLE - 0.2f), NULL)));

    triggers.push_back(new TriggerNode(
        "combat stuck",
        NextAction::array(0, new NextAction("reset", ACTION_IDLE - 0.3f), NULL)));
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
        NextAction::array(0, new NextAction("check mount state", ACTION_IDLE), new NextAction("check values", ACTION_IDLE), NULL)));

    triggers.push_back(new TriggerNode(
        "near dark portal",
        NextAction::array(0, new NextAction("move to dark portal", ACTION_IDLE), NULL)));

    triggers.push_back(new TriggerNode(
        "at dark portal azeroth",
        NextAction::array(0, new NextAction("use dark portal azeroth", ACTION_IDLE), NULL)));

    triggers.push_back(new TriggerNode(
        "at dark portal outland",
        NextAction::array(0, new NextAction("move from dark portal", ACTION_IDLE), NULL)));

    triggers.push_back(new TriggerNode(
        "need world buff",
        NextAction::array(0, new NextAction("world buff", ACTION_IDLE), NULL)));
}

void ClassStrategy::InitReactionTriggers(std::list<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode(
        "combat start",
        NextAction::array(0, new NextAction("set combat state", ACTION_EMERGENCY), NULL)));

    triggers.push_back(new TriggerNode(
        "combat end",
        NextAction::array(0, new NextAction("set non combat state", ACTION_EMERGENCY), NULL)));

    triggers.push_back(new TriggerNode(
        "death",
        NextAction::array(0, new NextAction("set dead state", ACTION_EMERGENCY), NULL)));

    triggers.push_back(new TriggerNode(
        "resurrect",
        NextAction::array(0, new NextAction("set non combat state", ACTION_EMERGENCY), NULL)));
}

void ClassStrategy::InitDeadTriggers(std::list<TriggerNode*>& triggers)
{

}

void ClassPvpStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode(
        "random",
        NextAction::array(0, new NextAction("free action potion", ACTION_HIGH), NULL)));
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

void ClassRaidStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{

}

void ClassRaidStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{

}

void ClassRaidStrategy::InitReactionTriggers(std::list<TriggerNode*>& triggers)
{

}

void ClassRaidStrategy::InitDeadTriggers(std::list<TriggerNode*>& triggers)
{

}

void AoeStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode(
        "light aoe",
        NextAction::array(0, new NextAction("goblin sapper", ACTION_HIGH + 1), NULL)));

    triggers.push_back(new TriggerNode(
        "light aoe",
        NextAction::array(0, new NextAction("adamantite grenade", ACTION_HIGH), NULL)));
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

void AoeRaidStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{

}

void AoeRaidStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
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

void CureRaidStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{

}

void CureRaidStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
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

void CcRaidStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{

}

void CcRaidStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{

}

void BuffStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode(
        "often",
        NextAction::array(0, new NextAction("use trinket", ACTION_HIGH), NULL)));
}

void BuffStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{

}

void BuffPvpStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{

}

void BuffPvpStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{

}

void BuffPveStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{

}

void BuffPveStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{

}

void BuffRaidStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{

}

void BuffRaidStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{

}

#elif MANGOSBOT_ONE // TBC

void ClassStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode(
        "invalid target",
        NextAction::array(0, new NextAction("select new target", ACTION_EMERGENCY), NULL)));

    triggers.push_back(new TriggerNode(
        "mounted",
        NextAction::array(0, new NextAction("check mount state", ACTION_EMERGENCY), NULL)));

    triggers.push_back(new TriggerNode(
        "very often",
        NextAction::array(0, new NextAction("use lightwell", ACTION_LIGHT_HEAL), NULL)));

    triggers.push_back(new TriggerNode(
        "combat long stuck",
        NextAction::array(0, new NextAction("hearthstone", ACTION_IDLE - 0.1f), new NextAction("repop", ACTION_IDLE - 0.2f), NULL)));

    triggers.push_back(new TriggerNode(
        "combat stuck",
        NextAction::array(0, new NextAction("reset", ACTION_IDLE - 0.3f), NULL)));
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
        NextAction::array(0, new NextAction("check mount state", ACTION_IDLE), new NextAction("check values", ACTION_IDLE), NULL)));

    triggers.push_back(new TriggerNode(
        "near dark portal",
        NextAction::array(0, new NextAction("move to dark portal", ACTION_IDLE), NULL)));

    triggers.push_back(new TriggerNode(
        "at dark portal azeroth",
        NextAction::array(0, new NextAction("use dark portal azeroth", ACTION_IDLE), NULL)));

    triggers.push_back(new TriggerNode(
        "at dark portal outland",
        NextAction::array(0, new NextAction("move from dark portal", ACTION_IDLE), NULL)));

    triggers.push_back(new TriggerNode(
        "need world buff",
        NextAction::array(0, new NextAction("world buff", ACTION_IDLE), NULL)));
}

void ClassStrategy::InitReactionTriggers(std::list<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode(
        "combat start",
        NextAction::array(0, new NextAction("set combat state", ACTION_EMERGENCY), NULL)));

    triggers.push_back(new TriggerNode(
        "combat end",
        NextAction::array(0, new NextAction("set non combat state", ACTION_EMERGENCY), NULL)));

    triggers.push_back(new TriggerNode(
        "death",
        NextAction::array(0, new NextAction("set dead state", ACTION_EMERGENCY), NULL)));

    triggers.push_back(new TriggerNode(
        "resurrect",
        NextAction::array(0, new NextAction("set non combat state", ACTION_EMERGENCY), NULL)));
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

void ClassRaidStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{

}

void ClassRaidStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{

}

void ClassRaidStrategy::InitReactionTriggers(std::list<TriggerNode*>& triggers)
{

}

void ClassRaidStrategy::InitDeadTriggers(std::list<TriggerNode*>& triggers)
{

}

void AoeStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode(
        "light aoe",
        NextAction::array(0, new NextAction("goblin sapper", ACTION_HIGH + 1), NULL)));

    triggers.push_back(new TriggerNode(
        "light aoe",
        NextAction::array(0, new NextAction("adamantite grenade", ACTION_HIGH), NULL)));
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

void AoeRaidStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{

}

void AoeRaidStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
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

void CureRaidStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{

}

void CureRaidStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
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

void CcRaidStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{

}

void CcRaidStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{

}

void BuffStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode(
        "often",
        NextAction::array(0, new NextAction("use trinket", ACTION_HIGH), NULL)));
}

void BuffStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{

}

void BuffPvpStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{

}

void BuffPvpStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{

}

void BuffPveStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{

}

void BuffPveStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{

}

void BuffRaidStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{

}

void BuffRaidStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{

}

#elif MANGOSBOT_TWO // WOTLK

void ClassStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode(
        "invalid target",
        NextAction::array(0, new NextAction("select new target", ACTION_EMERGENCY), NULL)));

    triggers.push_back(new TriggerNode(
        "mounted",
        NextAction::array(0, new NextAction("check mount state", ACTION_EMERGENCY), NULL)));

    triggers.push_back(new TriggerNode(
        "very often",
        NextAction::array(0, new NextAction("use lightwell", ACTION_LIGHT_HEAL), NULL)));

    triggers.push_back(new TriggerNode(
        "often",
        NextAction::array(0, new NextAction("use trinket", ACTION_HIGH), NULL)));

    triggers.push_back(new TriggerNode(
        "combat long stuck",
        NextAction::array(0, new NextAction("hearthstone", ACTION_IDLE - 0.1f), new NextAction("repop", ACTION_IDLE - 0.2f), NULL)));

    triggers.push_back(new TriggerNode(
        "combat stuck",
        NextAction::array(0, new NextAction("reset", ACTION_IDLE - 0.3f), NULL)));
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
        NextAction::array(0, new NextAction("check mount state", ACTION_IDLE), new NextAction("check values", ACTION_IDLE), NULL)));

    triggers.push_back(new TriggerNode(
        "near dark portal",
        NextAction::array(0, new NextAction("move to dark portal", ACTION_IDLE), NULL)));

    triggers.push_back(new TriggerNode(
        "at dark portal azeroth",
        NextAction::array(0, new NextAction("use dark portal azeroth", ACTION_IDLE), NULL)));

    triggers.push_back(new TriggerNode(
        "at dark portal outland",
        NextAction::array(0, new NextAction("move from dark portal", ACTION_IDLE), NULL)));

    triggers.push_back(new TriggerNode(
        "need world buff",
        NextAction::array(0, new NextAction("world buff", ACTION_IDLE), NULL)));
}

void ClassStrategy::InitReactionTriggers(std::list<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode(
        "combat start",
        NextAction::array(0, new NextAction("set combat state", ACTION_EMERGENCY), NULL)));

    triggers.push_back(new TriggerNode(
        "combat end",
        NextAction::array(0, new NextAction("set non combat state", ACTION_EMERGENCY), NULL)));

    triggers.push_back(new TriggerNode(
        "death",
        NextAction::array(0, new NextAction("set dead state", ACTION_EMERGENCY), NULL)));

    triggers.push_back(new TriggerNode(
        "resurrect",
        NextAction::array(0, new NextAction("set non combat state", ACTION_EMERGENCY), NULL)));
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

void ClassRaidStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{

}

void ClassRaidStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{

}

void ClassRaidStrategy::InitReactionTriggers(std::list<TriggerNode*>& triggers)
{

}

void ClassRaidStrategy::InitDeadTriggers(std::list<TriggerNode*>& triggers)
{

}

void AoeStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode(
        "light aoe",
        NextAction::array(0, new NextAction("goblin sapper", ACTION_HIGH + 1), NULL)));

    triggers.push_back(new TriggerNode(
        "light aoe",
        NextAction::array(0, new NextAction("adamantite grenade", ACTION_HIGH), NULL)));
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

void AoeRaidStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{

}

void AoeRaidStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
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

void CureRaidStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{

}

void CureRaidStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
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

void CcRaidStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{

}

void CcRaidStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{

}

void BuffStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode(
        "often",
        NextAction::array(0, new NextAction("use trinket", ACTION_HIGH), NULL)));
}

void BuffStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{

}

void BuffPvpStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{

}

void BuffPvpStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{

}

void BuffPveStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{

}

void BuffPveStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{

}

void BuffRaidStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{

}

void BuffRaidStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
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
        "in raid fight",
        NextAction::array(0, new NextAction("update raid strats", 100.0f), NULL)));
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
        "in raid fight",
        NextAction::array(0, new NextAction("update raid strats", 100.0f), NULL)));
}
