#include "botpch.h"
#include "../../playerbot.h"
#include "ShamanMultipliers.h"
#include "RestorationShamanStrategy.h"

using namespace ai;

class ShamanStrategyActionNodeFactory : public NamedObjectFactory<ActionNode>
{
public:
    ShamanStrategyActionNodeFactory()
    {
        creators["flametongue weapon"] = &flametongue_weapon;
        creators["frostbrand weapon"] = &frostbrand_weapon;
        creators["windfury weapon"] = &windfury_weapon;
        creators["lesser healing wave"] = &lesser_healing_wave;
        creators["lesser healing wave on party"] = &lesser_healing_wave_on_party;
        creators["chain heal"] = &chain_heal;
        creators["riptide"] = &riptide;
        creators["chain heal on party"] = &chain_heal_on_party;
        creators["riptide on party"] = &riptide_on_party;
        creators["earth shock"] = &earth_shock;
        creators["wind shear"] = &wind_shear;
    }

private:
    static ActionNode* wind_shear(PlayerbotAI* ai)
    {
        return new ActionNode("wind shear",
            /*P*/ NULL,
            /*A*/ NextAction::array(0, new NextAction("earth shock"), NULL),
            /*C*/ NULL);
    }

    static ActionNode* earth_shock(PlayerbotAI* ai)
    {
        return new ActionNode ("earth shock",
            /*P*/ NULL,
            /*A*/ NextAction::array(0, new NextAction("flame shock"), NULL),
            /*C*/ NULL);
    }

    static ActionNode* flametongue_weapon(PlayerbotAI* ai)
    {
        return new ActionNode ("flametongue weapon",
            /*P*/ NULL,
            /*A*/ NextAction::array(0, new NextAction("frostbrand weapon"), NULL),
            /*C*/ NULL);
    }

    static ActionNode* frostbrand_weapon(PlayerbotAI* ai)
    {
        return new ActionNode ("frostbrand weapon",
            /*P*/ NULL,
            /*A*/ NextAction::array(0, new NextAction("rockbiter weapon"), NULL),
            /*C*/ NULL);
    }

    static ActionNode* windfury_weapon(PlayerbotAI* ai)
    {
        return new ActionNode ("windfury weapon",
            /*P*/ NULL,
            /*A*/ NextAction::array(0, new NextAction("rockbiter weapon"), NULL),
            /*C*/ NULL);
    }

    static ActionNode* lesser_healing_wave(PlayerbotAI* ai)
    {
        return new ActionNode ("lesser healing wave",
            /*P*/ NULL,
            /*A*/ NextAction::array(0, new NextAction("healing wave"), NULL),
            /*C*/ NULL);
    }

    static ActionNode* lesser_healing_wave_on_party(PlayerbotAI* ai)
    {
        return new ActionNode ("lesser healing wave on party",
            /*P*/ NULL,
            /*A*/ NextAction::array(0, new NextAction("healing wave on party"), NULL),
            /*C*/ NULL);
    }

    static ActionNode* chain_heal(PlayerbotAI* ai)
    {
        return new ActionNode ("chain heal",
            /*P*/ NULL,
            /*A*/ NextAction::array(0, new NextAction("lesser healing wave"), NULL),
            /*C*/ NULL);
    }

    static ActionNode* riptide(PlayerbotAI* ai)
    {
        return new ActionNode ("riptide",
            /*P*/ NULL,
            /*A*/ NextAction::array(0, new NextAction("healing wave"), NULL),
            /*C*/ NULL);
    }

    static ActionNode* chain_heal_on_party(PlayerbotAI* ai)
    {
        return new ActionNode ("chain heal on party",
            /*P*/ NULL,
            /*A*/ NextAction::array(0, new NextAction("lesser healing wave on party"), NULL),
            /*C*/ NULL);
    }

    static ActionNode* riptide_on_party(PlayerbotAI* ai)
    {
        return new ActionNode ("riptide on party",
            /*P*/ NULL,
            /*A*/ NextAction::array(0, new NextAction("healing wave on party"), NULL),
            /*C*/ NULL);
    }
};

ShamanStrategy::ShamanStrategy(PlayerbotAI* ai) : ClassStrategy(ai)
{
    actionNodeFactories.Add(new ShamanStrategyActionNodeFactory());
}

#ifdef MANGOSBOT_ZERO // Vanilla

void ShamanStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    ClassStrategy::InitCombatTriggers(triggers);

    triggers.push_back(new TriggerNode(
        "purge",
        NextAction::array(0, new NextAction("purge", 80.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "medium aoe heal",
        NextAction::array(0, new NextAction("chain heal", 27.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "low health",
        NextAction::array(0, new NextAction("riptide", 26.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "medium health",
        NextAction::array(0, new NextAction("lesser healing wave", 26.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "party member low health",
        NextAction::array(0, new NextAction("riptide on party", 25.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "party member medium health",
        NextAction::array(0, new NextAction("lesser healing wave on party", 25.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "wind shear",
        NextAction::array(0, new NextAction("wind shear", 23.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "wind shear on enemy healer",
        NextAction::array(0, new NextAction("wind shear on enemy healer", 23.0f), NULL)));
}

void ShamanStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{
    ClassStrategy::InitNonCombatTriggers(triggers);

    triggers.push_back(new TriggerNode(
        "critical health",
        NextAction::array(0, new NextAction("healing wave", 70.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "party member critical health",
        NextAction::array(0, new NextAction("healing wave on party", 60.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "party member dead",
        NextAction::array(0, new NextAction("ancestral spirit", 33.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "cure disease",
        NextAction::array(0, new NextAction("cure disease", 31.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "party member cure disease",
        NextAction::array(0, new NextAction("cure disease on party", 30.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "medium aoe heal",
        NextAction::array(0, new NextAction("chain heal", 27.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "cure poison",
        NextAction::array(0, new NextAction("cure poison", 21.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "party member cure poison",
        NextAction::array(0, new NextAction("cure poison on party", 21.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "water breathing",
        NextAction::array(0, new NextAction("water breathing", 12.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "water walking",
        NextAction::array(0, new NextAction("water walking", 12.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "water breathing on party",
        NextAction::array(0, new NextAction("water breathing on party", 11.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "water walking on party",
        NextAction::array(0, new NextAction("water walking on party", 11.0f), NULL)));
}

void ShamanStrategy::InitReactionTriggers(std::list<TriggerNode*>& triggers)
{
    ClassStrategy::InitReactionTriggers(triggers);
}

void ShamanStrategy::InitDeadTriggers(std::list<TriggerNode*>& triggers)
{
    ClassStrategy::InitDeadTriggers(triggers);
}

void ShamanPvpStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    ClassPvpStrategy::InitCombatTriggers(triggers);

    triggers.push_back(new TriggerNode(
        "player has flag",
        NextAction::array(0, new NextAction("ghost wolf", ACTION_EMERGENCY + 2), NULL)));
}

void ShamanPvpStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{
    ClassPvpStrategy::InitNonCombatTriggers(triggers);

    triggers.push_back(new TriggerNode(
        "player has flag",
        NextAction::array(0, new NextAction("ghost wolf", ACTION_EMERGENCY + 2), NULL)));
}

void ShamanPvpStrategy::InitReactionTriggers(std::list<TriggerNode*>& triggers)
{
    ClassPvpStrategy::InitReactionTriggers(triggers);
}

void ShamanPvpStrategy::InitDeadTriggers(std::list<TriggerNode*>& triggers)
{
    ClassPvpStrategy::InitDeadTriggers(triggers);
}

void ShamanPveStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    ClassPveStrategy::InitCombatTriggers(triggers);
}

void ShamanPveStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{
    ClassPveStrategy::InitNonCombatTriggers(triggers);
}

void ShamanPveStrategy::InitReactionTriggers(std::list<TriggerNode*>& triggers)
{
    ClassPveStrategy::InitReactionTriggers(triggers);
}

void ShamanPveStrategy::InitDeadTriggers(std::list<TriggerNode*>& triggers)
{
    ClassPveStrategy::InitDeadTriggers(triggers);
}

void ShamanAoeStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    AoeStrategy::InitCombatTriggers(triggers);
}

void ShamanAoeStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{
    AoeStrategy::InitNonCombatTriggers(triggers);
}

void ShamanAoePvpStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    AoePvpStrategy::InitCombatTriggers(triggers);
}

void ShamanAoePvpStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{
    AoePvpStrategy::InitNonCombatTriggers(triggers);
}

void ShamanAoePveStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    AoePveStrategy::InitCombatTriggers(triggers);
}

void ShamanAoePveStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{
    AoePveStrategy::InitNonCombatTriggers(triggers);
}

void ShamanBuffDpsStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode(
        "lightning shield",
        NextAction::array(0, new NextAction("lightning shield", 22.0f), NULL)));
}

void ShamanBuffDpsStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{
    InitCombatTriggers(triggers);
}

void ShamanBuffManaStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    
}

void ShamanBuffManaStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{
    InitCombatTriggers(triggers);
}

void ShamanCureStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    CureStrategy::InitCombatTriggers(triggers);

    triggers.push_back(new TriggerNode(
        "cure disease",
        NextAction::array(0, new NextAction("cure disease", 31.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "party member cure disease",
        NextAction::array(0, new NextAction("cure disease on party", 30.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "cure poison",
        NextAction::array(0, new NextAction("cure poison", 21.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "party member cure poison",
        NextAction::array(0, new NextAction("cure poison on party", 21.0f), NULL)));
}

void ShamanCureStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{
    CureStrategy::InitNonCombatTriggers(triggers);

    triggers.push_back(new TriggerNode(
        "cure disease",
        NextAction::array(0, new NextAction("cure disease", 31.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "party member cure disease",
        NextAction::array(0, new NextAction("cure disease on party", 30.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "cure poison",
        NextAction::array(0, new NextAction("cure poison", 21.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "party member cure poison",
        NextAction::array(0, new NextAction("cure poison on party", 21.0f), NULL)));
}

void ShamanCurePvpStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    CurePvpStrategy::InitCombatTriggers(triggers);
}

void ShamanCurePvpStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{
    CurePvpStrategy::InitNonCombatTriggers(triggers);
}

void ShamanCurePveStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    CurePveStrategy::InitCombatTriggers(triggers);
}

void ShamanCurePveStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{
    CurePveStrategy::InitNonCombatTriggers(triggers);
}

void ShamanTotemsStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{

}

void ShamanTotemsStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{

}

void ShamanTotemsPvpStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{

}

void ShamanTotemsPvpStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{

}

void ShamanTotemsPveStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{

}

void ShamanTotemsPveStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{

}

void ShamanTotemBarElementsStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{

}

void ShamanTotemBarAncestorsStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{

}

void ShamanTotemBarSpiritsStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{

}

#elif MANGOSBOT_ONE // TBC

void ShamanStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    ClassStrategy::InitCombatTriggers(triggers);

    triggers.push_back(new TriggerNode(
        "purge",
        NextAction::array(0, new NextAction("purge", 80.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "heroism",
        NextAction::array(0, new NextAction("heroism", 31.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "bloodlust",
        NextAction::array(0, new NextAction("bloodlust", 30.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "medium aoe heal",
        NextAction::array(0, new NextAction("chain heal", 27.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "low health",
        NextAction::array(0, new NextAction("riptide", 26.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "medium health",
        NextAction::array(0, new NextAction("lesser healing wave", 26.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "party member low health",
        NextAction::array(0, new NextAction("riptide on party", 25.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "party member medium health",
        NextAction::array(0, new NextAction("lesser healing wave on party", 25.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "wind shear",
        NextAction::array(0, new NextAction("wind shear", 23.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "wind shear on enemy healer",
        NextAction::array(0, new NextAction("wind shear on enemy healer", 23.0f), NULL)));
}

void ShamanStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{
    ClassStrategy::InitNonCombatTriggers(triggers);

    triggers.push_back(new TriggerNode(
        "critical health",
        NextAction::array(0, new NextAction("healing wave", 70.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "party member critical health",
        NextAction::array(0, new NextAction("healing wave on party", 60.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "party member dead",
        NextAction::array(0, new NextAction("ancestral spirit", 33.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "cure disease",
        NextAction::array(0, new NextAction("cure disease", 31.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "party member cure disease",
        NextAction::array(0, new NextAction("cure disease on party", 30.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "medium aoe heal",
        NextAction::array(0, new NextAction("chain heal", 27.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "cure poison",
        NextAction::array(0, new NextAction("cure poison", 21.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "party member cure poison",
        NextAction::array(0, new NextAction("cure poison on party", 21.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "water breathing",
        NextAction::array(0, new NextAction("water breathing", 12.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "water walking",
        NextAction::array(0, new NextAction("water walking", 12.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "water breathing on party",
        NextAction::array(0, new NextAction("water breathing on party", 11.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "water walking on party",
        NextAction::array(0, new NextAction("water walking on party", 11.0f), NULL)));
}

void ShamanStrategy::InitReactionTriggers(std::list<TriggerNode*>& triggers)
{
    ClassStrategy::InitReactionTriggers(triggers);
}

void ShamanStrategy::InitDeadTriggers(std::list<TriggerNode*>& triggers)
{
    ClassStrategy::InitDeadTriggers(triggers);
}

void ShamanPvpStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    ClassPvpStrategy::InitCombatTriggers(triggers);

    triggers.push_back(new TriggerNode(
        "player has flag",
        NextAction::array(0, new NextAction("ghost wolf", ACTION_EMERGENCY + 2), NULL)));
}

void ShamanPvpStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{
    ClassPvpStrategy::InitNonCombatTriggers(triggers);

    triggers.push_back(new TriggerNode(
        "player has flag",
        NextAction::array(0, new NextAction("ghost wolf", ACTION_EMERGENCY + 2), NULL)));
}

void ShamanPvpStrategy::InitReactionTriggers(std::list<TriggerNode*>& triggers)
{
    ClassPvpStrategy::InitReactionTriggers(triggers);
}

void ShamanPvpStrategy::InitDeadTriggers(std::list<TriggerNode*>& triggers)
{
    ClassPvpStrategy::InitDeadTriggers(triggers);
}

void ShamanPveStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    ClassPveStrategy::InitCombatTriggers(triggers);
}

void ShamanPveStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{
    ClassPveStrategy::InitNonCombatTriggers(triggers);
}

void ShamanPveStrategy::InitReactionTriggers(std::list<TriggerNode*>& triggers)
{
    ClassPveStrategy::InitReactionTriggers(triggers);
}

void ShamanPveStrategy::InitDeadTriggers(std::list<TriggerNode*>& triggers)
{
    ClassPveStrategy::InitDeadTriggers(triggers);
}

void ShamanAoeStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    AoeStrategy::InitCombatTriggers(triggers);
}

void ShamanAoeStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{
    AoeStrategy::InitNonCombatTriggers(triggers);
}

void ShamanAoePvpStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    AoePvpStrategy::InitCombatTriggers(triggers);
}

void ShamanAoePvpStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{
    AoePvpStrategy::InitNonCombatTriggers(triggers);
}

void ShamanAoePveStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    AoePveStrategy::InitCombatTriggers(triggers);
}

void ShamanAoePveStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{
    AoePveStrategy::InitNonCombatTriggers(triggers);
}

void ShamanBuffDpsStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode(
        "lightning shield",
        NextAction::array(0, new NextAction("lightning shield", 22.0f), NULL)));
}

void ShamanBuffDpsStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{
    InitCombatTriggers(triggers);
}

void ShamanBuffManaStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode(
        "water shield",
        NextAction::array(0, new NextAction("water shield", 22.0f), NULL)));
}

void ShamanBuffManaStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{
    InitCombatTriggers(triggers);
}

void ShamanCureStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    CureStrategy::InitCombatTriggers(triggers);

    triggers.push_back(new TriggerNode(
        "cure disease",
        NextAction::array(0, new NextAction("cure disease", 31.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "party member cure disease",
        NextAction::array(0, new NextAction("cure disease on party", 30.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "cure poison",
        NextAction::array(0, new NextAction("cure poison", 21.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "party member cure poison",
        NextAction::array(0, new NextAction("cure poison on party", 21.0f), NULL)));
}

void ShamanCureStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{
    CureStrategy::InitNonCombatTriggers(triggers);

    triggers.push_back(new TriggerNode(
        "cure disease",
        NextAction::array(0, new NextAction("cure disease", 31.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "party member cure disease",
        NextAction::array(0, new NextAction("cure disease on party", 30.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "cure poison",
        NextAction::array(0, new NextAction("cure poison", 21.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "party member cure poison",
        NextAction::array(0, new NextAction("cure poison on party", 21.0f), NULL)));
}

void ShamanCurePvpStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    CurePvpStrategy::InitCombatTriggers(triggers);
}

void ShamanCurePvpStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{
    CurePvpStrategy::InitNonCombatTriggers(triggers);
}

void ShamanCurePveStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    CurePveStrategy::InitCombatTriggers(triggers);
}

void ShamanCurePveStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{
    CurePveStrategy::InitNonCombatTriggers(triggers);
}

void ShamanTotemsStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{

}

void ShamanTotemsStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{

}

void ShamanTotemsPvpStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{

}

void ShamanTotemsPvpStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{

}

void ShamanTotemsPveStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{

}

void ShamanTotemsPveStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{

}

void ShamanTotemBarElementsStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{

}

void ShamanTotemBarAncestorsStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{

}

void ShamanTotemBarSpiritsStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{

}

#elif MANGOSBOT_TWO // WOTLK

void ShamanStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    ClassStrategy::InitCombatTriggers(triggers);

    triggers.push_back(new TriggerNode(
        "purge",
        NextAction::array(0, new NextAction("purge", 80.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "heroism",
        NextAction::array(0, new NextAction("heroism", 31.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "bloodlust",
        NextAction::array(0, new NextAction("bloodlust", 30.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "medium aoe heal",
        NextAction::array(0, new NextAction("chain heal", 27.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "low health",
        NextAction::array(0, new NextAction("riptide", 26.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "medium health",
        NextAction::array(0, new NextAction("lesser healing wave", 26.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "party member low health",
        NextAction::array(0, new NextAction("riptide on party", 25.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "party member medium health",
        NextAction::array(0, new NextAction("lesser healing wave on party", 25.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "wind shear",
        NextAction::array(0, new NextAction("wind shear", 23.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "wind shear on enemy healer",
        NextAction::array(0, new NextAction("wind shear on enemy healer", 23.0f), NULL)));
}

void ShamanStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{
    ClassStrategy::InitNonCombatTriggers(triggers);

    triggers.push_back(new TriggerNode(
        "critical health",
        NextAction::array(0, new NextAction("healing wave", 70.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "party member critical health",
        NextAction::array(0, new NextAction("healing wave on party", 60.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "party member dead",
        NextAction::array(0, new NextAction("ancestral spirit", 33.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "cure disease",
        NextAction::array(0, new NextAction("cure disease", 31.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "party member cure disease",
        NextAction::array(0, new NextAction("cure disease on party", 30.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "medium aoe heal",
        NextAction::array(0, new NextAction("chain heal", 27.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "cure poison",
        NextAction::array(0, new NextAction("cure poison", 21.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "party member cure poison",
        NextAction::array(0, new NextAction("cure poison on party", 21.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "totemic recall",
        NextAction::array(0, new NextAction("totemic recall", 15.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "water breathing",
        NextAction::array(0, new NextAction("water breathing", 12.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "water walking",
        NextAction::array(0, new NextAction("water walking", 12.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "water breathing on party",
        NextAction::array(0, new NextAction("water breathing on party", 11.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "water walking on party",
        NextAction::array(0, new NextAction("water walking on party", 11.0f), NULL)));
}

void ShamanStrategy::InitReactionTriggers(std::list<TriggerNode*>& triggers)
{
    ClassStrategy::InitReactionTriggers(triggers);
}

void ShamanStrategy::InitDeadTriggers(std::list<TriggerNode*>& triggers)
{
    ClassStrategy::InitDeadTriggers(triggers);
}

void ShamanPvpStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    ClassPvpStrategy::InitCombatTriggers(triggers);

    triggers.push_back(new TriggerNode(
        "player has flag",
        NextAction::array(0, new NextAction("ghost wolf", ACTION_EMERGENCY + 2), NULL)));
}

void ShamanPvpStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{
    ClassPvpStrategy::InitNonCombatTriggers(triggers);

    triggers.push_back(new TriggerNode(
        "player has flag",
        NextAction::array(0, new NextAction("ghost wolf", ACTION_EMERGENCY + 2), NULL)));
}

void ShamanPvpStrategy::InitReactionTriggers(std::list<TriggerNode*>& triggers)
{
    ClassPvpStrategy::InitReactionTriggers(triggers);
}

void ShamanPvpStrategy::InitDeadTriggers(std::list<TriggerNode*>& triggers)
{
    ClassPvpStrategy::InitDeadTriggers(triggers);
}

void ShamanPveStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    ClassPveStrategy::InitCombatTriggers(triggers);
}

void ShamanPveStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{
    ClassPveStrategy::InitNonCombatTriggers(triggers);
}

void ShamanPveStrategy::InitReactionTriggers(std::list<TriggerNode*>& triggers)
{
    ClassPveStrategy::InitReactionTriggers(triggers);
}

void ShamanPveStrategy::InitDeadTriggers(std::list<TriggerNode*>& triggers)
{
    ClassPveStrategy::InitDeadTriggers(triggers);
}

void ShamanAoeStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    AoeStrategy::InitCombatTriggers(triggers);
}

void ShamanAoeStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{
    AoeStrategy::InitNonCombatTriggers(triggers);
}

void ShamanAoePvpStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    AoePvpStrategy::InitCombatTriggers(triggers);
}

void ShamanAoePvpStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{
    AoePvpStrategy::InitNonCombatTriggers(triggers);
}

void ShamanAoePveStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    AoePveStrategy::InitCombatTriggers(triggers);
}

void ShamanAoePveStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{
    AoePveStrategy::InitNonCombatTriggers(triggers);
}

void ShamanBuffDpsStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode(
        "lightning shield",
        NextAction::array(0, new NextAction("lightning shield", 22.0f), NULL)));
}

void ShamanBuffDpsStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{
    InitCombatTriggers(triggers);
}

void ShamanBuffManaStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode(
        "water shield",
        NextAction::array(0, new NextAction("water shield", 22.0f), NULL)));
}

void ShamanBuffManaStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{
    InitCombatTriggers(triggers);
}

void ShamanCureStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    CureStrategy::InitCombatTriggers(triggers);

    triggers.push_back(new TriggerNode(
        "cure disease",
        NextAction::array(0, new NextAction("cure disease", 31.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "party member cure disease",
        NextAction::array(0, new NextAction("cure disease on party", 30.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "cleanse spirit poison",
        NextAction::array(0, new NextAction("cleanse spirit", 24.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "cleanse spirit disease",
        NextAction::array(0, new NextAction("cleanse spirit", 24.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "cleanse spirit curse",
        NextAction::array(0, new NextAction("cleanse spirit", 24.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "party member cleanse spirit poison",
        NextAction::array(0, new NextAction("cleanse spirit poison on party", 23.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "party member cleanse spirit disease",
        NextAction::array(0, new NextAction("cleanse spirit disease on party", 23.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "party member cleanse spirit curse",
        NextAction::array(0, new NextAction("cleanse spirit curse on party", 23.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "cure poison",
        NextAction::array(0, new NextAction("cure poison", 21.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "party member cure poison",
        NextAction::array(0, new NextAction("cure poison on party", 21.0f), NULL)));

    if (sRandomPlayerbotMgr.IsRandomBot(ai->GetBot()))
    {
        triggers.push_back(new TriggerNode(
            "levelup",
            NextAction::array(0, new NextAction("set totembars on levelup", 20.0f), NULL)));
    }
}

void ShamanCureStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{
    CureStrategy::InitNonCombatTriggers(triggers);

    triggers.push_back(new TriggerNode(
        "cure disease",
        NextAction::array(0, new NextAction("cure disease", 31.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "party member cure disease",
        NextAction::array(0, new NextAction("cure disease on party", 30.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "cleanse spirit poison",
        NextAction::array(0, new NextAction("cleanse spirit", 24.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "cleanse spirit disease",
        NextAction::array(0, new NextAction("cleanse spirit", 24.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "cleanse spirit curse",
        NextAction::array(0, new NextAction("cleanse spirit", 24.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "party member cleanse spirit poison",
        NextAction::array(0, new NextAction("cleanse spirit poison on party", 23.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "party member cleanse spirit disease",
        NextAction::array(0, new NextAction("cleanse spirit disease on party", 23.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "party member cleanse spirit curse",
        NextAction::array(0, new NextAction("cleanse spirit curse on party", 23.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "cure poison",
        NextAction::array(0, new NextAction("cure poison", 21.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "party member cure poison",
        NextAction::array(0, new NextAction("cure poison on party", 21.0f), NULL)));

    if (sRandomPlayerbotMgr.IsRandomBot(ai->GetBot()))
    {
        triggers.push_back(new TriggerNode(
            "levelup",
            NextAction::array(0, new NextAction("set totembars on levelup", 20.0f), NULL)));
    }
}

void ShamanCurePvpStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    CurePvpStrategy::InitCombatTriggers(triggers);
}

void ShamanCurePvpStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{
    CurePvpStrategy::InitNonCombatTriggers(triggers);
}

void ShamanCurePveStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    CurePveStrategy::InitCombatTriggers(triggers);
}

void ShamanCurePveStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{
    CurePveStrategy::InitNonCombatTriggers(triggers);
}

void ShamanTotemsStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{

}

void ShamanTotemsStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{

}

void ShamanTotemsPvpStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{

}

void ShamanTotemsPvpStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{

}

void ShamanTotemsPveStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{

}

void ShamanTotemsPveStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{

}

void ShamanTotemBarElementsStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode(
        "call of the elements",
        NextAction::array(0, new NextAction("call of the elements", 30.0f), NULL)));
}

void ShamanTotemBarAncestorsStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode(
        "call of the ancestors",
        NextAction::array(0, new NextAction("call of the ancestors", 30.0f), NULL)));
}

void ShamanTotemBarSpiritsStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode(
        "call of the ancestors",
        NextAction::array(0, new NextAction("call of the spirits", 30.0f), NULL)));
}

#endif