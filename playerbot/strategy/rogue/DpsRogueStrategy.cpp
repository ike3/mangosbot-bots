#include "botpch.h"
#include "../../playerbot.h"
#include "RogueMultipliers.h"
#include "DpsRogueStrategy.h"

using namespace ai;

class DpsRogueStrategyActionNodeFactory : public NamedObjectFactory<ActionNode>
{
public:
    DpsRogueStrategyActionNodeFactory()
    {
        creators["riposte"] = &riposte;
        creators["mutilate"] = &mutilate;
        creators["mutilate front"] = &mutilate_front;
        creators["mutilate back"] = &mutilate_back;
        creators["sinister strike"] = &sinister_strike;
        creators["kick"] = &kick;
        creators["kidney shot"] = &kidney_shot;
        creators["slice and dice"] = &slice_and_dice;
        creators["eviscerate"] = &eviscerate;
        creators["backstab"] = &backstab;
        creators["ambush"] = &ambush;
        creators["hemorrhage back"] = &hemorrhage_back;
        creators["hemorrhage front"] = &hemorrhage_front;
    }
private:
    static ActionNode* riposte(PlayerbotAI* ai)
    {
        return new ActionNode("riposte",
            /*P*/ NULL,
            /*A*/ NextAction::array(0, new NextAction("mutilate"), NULL),
            /*C*/ NULL);
    }
    ACTION_NODE_A(mutilate_front, "mutilate", "sinister strike");
    ACTION_NODE_A(mutilate_back, "mutilate", "backstab");
    ACTION_NODE_A(mutilate, "mutilate", "backstab");
    ACTION_NODE_A(backstab, "backstab", "melee");
    ACTION_NODE_A(sinister_strike, "sinister strike", "melee");
    ACTION_NODE_A(hemorrhage_back, "hemorrhage", "backstab");
    ACTION_NODE_A(hemorrhage_front, "hemorrhage", "sinister strike");
    static ActionNode* kick(PlayerbotAI* ai)
    {
        return new ActionNode("kick",
            /*P*/ NULL,
            /*A*/ NextAction::array(0, new NextAction("kidney shot"), NULL),
            /*C*/ NULL);
    }
    ACTION_NODE_A(kidney_shot, "kidney shot", "gouge");
    static ActionNode* eviscerate(PlayerbotAI* ai)
    {
        return new ActionNode("eviscerate",
            /*P*/ NULL,
            /*A*/ NextAction::array(0, new NextAction("rupture"), NULL),
            /*C*/ NULL);
    }
    static ActionNode* ambush(PlayerbotAI* ai)
    {
        return new ActionNode("ambush",
            /*P*/ NULL,
            /*A*/ NextAction::array(0, new NextAction("cheap shot"), NULL),
            /*C*/ NULL);
    }
    ACTION_NODE_A(slice_and_dice, "slice and dice", "eviscerate");
};

GenericRogueStrategy::GenericRogueStrategy(PlayerbotAI* ai) : CombatStrategy(ai)
{
    actionNodeFactories.Add(new DpsRogueStrategyActionNodeFactory());
}

void GenericRogueStrategy::InitTriggers(std::list<TriggerNode*>& triggers)
{
    CombatStrategy::InitTriggers(triggers);

    triggers.push_back(new TriggerNode(
        "medium threat",
        NextAction::array(0, new NextAction("feint", ACTION_HIGH), NULL)));

    triggers.push_back(new TriggerNode(
        "medium health",
        NextAction::array(0, new NextAction("gouge", ACTION_HIGH), NULL)));

    triggers.push_back(new TriggerNode(
        "low health",
        NextAction::array(0, new NextAction("evasion", ACTION_EMERGENCY), new NextAction("feint", ACTION_EMERGENCY), NULL)));

    triggers.push_back(new TriggerNode(
        "low health",
        NextAction::array(0, new NextAction("blind", ACTION_EMERGENCY + 2), new NextAction("vanish", ACTION_EMERGENCY + 1), new NextAction("vanish", ACTION_EMERGENCY), NULL)));

    triggers.push_back(new TriggerNode(
        "kick",
        NextAction::array(0, new NextAction("kick", ACTION_INTERRUPT + 5), NULL)));

    triggers.push_back(new TriggerNode(
        "kick on enemy healer",
        NextAction::array(0, new NextAction("kick on enemy healer", ACTION_INTERRUPT + 6), NULL)));

    triggers.push_back(new TriggerNode(
        "player has flag",
        NextAction::array(0, new NextAction("sprint", ACTION_EMERGENCY + 2), NULL)));

    triggers.push_back(new TriggerNode(
        "enemy flagcarrier near",
        NextAction::array(0, new NextAction("sprint", ACTION_EMERGENCY + 1), NULL)));

    triggers.push_back(new TriggerNode(
        "in stealth",
        NextAction::array(0, new NextAction("check stealth", ACTION_EMERGENCY), NULL)));

    triggers.push_back(new TriggerNode(
        "unstealth",
        NextAction::array(0, new NextAction("unstealth", ACTION_INTERRUPT + 1), NULL)));

    triggers.push_back(new TriggerNode(
        "sprint",
        NextAction::array(0, new NextAction("sprint", ACTION_HIGH + 10), NULL)));

    triggers.push_back(new TriggerNode(
        "very often",
        NextAction::array(0, new NextAction("cold blood", ACTION_HIGH), NULL)));

    triggers.push_back(new TriggerNode(
        "enemy out of melee",
        NextAction::array(0, new NextAction("adamantite grenade", ACTION_HIGH), NULL)));

    triggers.push_back(new TriggerNode(
        "cloak of shadows",
        NextAction::array(0, new NextAction("cloak of shadows", ACTION_INTERRUPT + 5), NULL)));

    /*triggers.push_back(new TriggerNode(
        "tricks of the trade on tank",
        NextAction::array(0, new NextAction("tricks of the trade on tank", ACTION_HIGH), NULL)));*/
}

DpsRogueStrategy::DpsRogueStrategy(PlayerbotAI* ai) : GenericRogueStrategy(ai)
{
}

void DpsRogueStrategy::InitTriggers(std::list<TriggerNode*>& triggers)
{
    GenericRogueStrategy::InitTriggers(triggers);

    triggers.push_back(new TriggerNode(
        "slice and dice",
        NextAction::array(0, new NextAction("slice and dice", ACTION_HIGH + 5), NULL)));

    triggers.push_back(new TriggerNode(
        "rupture",
        NextAction::array(0, new NextAction("rupture", ACTION_HIGH + 4), NULL)));

    triggers.push_back(new TriggerNode(
        "eviscerate",
        NextAction::array(0, new NextAction("eviscerate", ACTION_HIGH + 3), NULL)));

    triggers.push_back(new TriggerNode(
        "sinister strike",
        NextAction::array(0, new NextAction("sinister strike", ACTION_NORMAL + 5), NULL)));
}

// Assassination
AssassinationRogueStrategy::AssassinationRogueStrategy(PlayerbotAI* ai) : GenericRogueStrategy(ai)
{
}

void AssassinationRogueStrategy::InitTriggers(std::list<TriggerNode*>& triggers)
{
    GenericRogueStrategy::InitTriggers(triggers);

    triggers.push_back(new TriggerNode(
        "3 combo",
        NextAction::array(0, new NextAction("rupture", ACTION_HIGH + 1), NULL)));

    triggers.push_back(new TriggerNode(
        "eviscerate",
        NextAction::array(0, new NextAction("eviscerate", ACTION_HIGH + 3), NULL)));

    triggers.push_back(new TriggerNode(
        "slice and dice",
        NextAction::array(0, new NextAction("slice and dice", ACTION_HIGH + 2), NULL)));

    triggers.push_back(new TriggerNode(
        "behind target",
        NextAction::array(0, new NextAction("backstab", ACTION_HIGH + 3), NULL)));

    triggers.push_back(new TriggerNode(
        "behind target",
        NextAction::array(0, new NextAction("mutilate back", ACTION_HIGH + 4), NULL)));

    triggers.push_back(new TriggerNode(
        "not behind target",
        NextAction::array(0, new NextAction("mutilate front", ACTION_HIGH + 4), NULL)));
}

// Combat
CombatRogueStrategy::CombatRogueStrategy(PlayerbotAI* ai) : GenericRogueStrategy(ai)
{
}

void CombatRogueStrategy::InitTriggers(std::list<TriggerNode*>& triggers)
{
    GenericRogueStrategy::InitTriggers(triggers);

    //triggers.push_back(new TriggerNode(
    //    "blade flurry with killing spree",
    //    NextAction::array(0, new NextAction("blade flurry", ACTION_NORMAL + 9), new NextAction("killing spree", ACTION_NORMAL + 8), NULL)));

    triggers.push_back(new TriggerNode(
        "killing spree",
        NextAction::array(0, new NextAction("killing spree", ACTION_NORMAL + 8), NULL)));

    triggers.push_back(new TriggerNode(
        "sinister strike",
        NextAction::array(0, new NextAction("sinister strike", ACTION_NORMAL), NULL)));

    triggers.push_back(new TriggerNode(
        "slice and dice",
        NextAction::array(0, new NextAction("slice and dice", ACTION_HIGH + 5), NULL)));

    triggers.push_back(new TriggerNode(
        "rupture",
        NextAction::array(0, new NextAction("rupture", ACTION_HIGH + 3), NULL)));

    triggers.push_back(new TriggerNode(
        "eviscerate",
        NextAction::array(0, new NextAction("eviscerate", ACTION_HIGH + 4), NULL)));

    triggers.push_back(new TriggerNode(
        "riposte",
        NextAction::array(0, new NextAction("riposte", ACTION_HIGH + 5), NULL)));
}

// Subtlety
SubtletyRogueStrategy::SubtletyRogueStrategy(PlayerbotAI* ai) : GenericRogueStrategy(ai)
{
}

void SubtletyRogueStrategy::InitTriggers(std::list<TriggerNode*>& triggers)
{
    GenericRogueStrategy::InitTriggers(triggers);

    triggers.push_back(new TriggerNode(
        "behind target",
        NextAction::array(0, new NextAction("hemorrhage back", ACTION_HIGH + 4), NULL)));

    triggers.push_back(new TriggerNode(
        "not behind target",
        NextAction::array(0, new NextAction("hemorrhage front", ACTION_HIGH + 4), NULL)));

    /*triggers.push_back(new TriggerNode(
        "hemorrhage",
        NextAction::array(0, new NextAction("hemorrhage or backstab", ACTION_NORMAL + 4), NULL)));*/

    triggers.push_back(new TriggerNode(
        "very often",
        NextAction::array(0, new NextAction("ghostly strike", ACTION_NORMAL + 4), NULL)));

    triggers.push_back(new TriggerNode(
        "slice and dice",
        NextAction::array(0, new NextAction("slice and dice", ACTION_HIGH + 1), NULL)));

    triggers.push_back(new TriggerNode(
        "eviscerate",
        NextAction::array(0, new NextAction("eviscerate", ACTION_HIGH + 3), NULL)));

    triggers.push_back(new TriggerNode(
        "rupture",
        NextAction::array(0, new NextAction("rupture", ACTION_HIGH + 2), NULL)));

    triggers.push_back(new TriggerNode(
        "random",
        NextAction::array(0, new NextAction("preparation", ACTION_HIGH + 1), NULL)));

    triggers.push_back(new TriggerNode(
        "very often",
        NextAction::array(0, new NextAction("premeditation", ACTION_HIGH + 1), NULL)));

    triggers.push_back(new TriggerNode(
        "enemy out of melee",
        NextAction::array(0, new NextAction("shadowstep", 62.0f), NULL)));
}

class StealthedRogueStrategyActionNodeFactory : public NamedObjectFactory<ActionNode>
{
public:
    StealthedRogueStrategyActionNodeFactory()
    {
        creators["ambush"] = &ambush;
        creators["backstab"] = &backstab;
        creators["cheap shot"] = &cheap_shot;
    }
private:
    static ActionNode* ambush(PlayerbotAI* ai)
    {
        return new ActionNode("ambush",
            /*P*/ NULL,
            /*A*/ NextAction::array(0, new NextAction("backstab"), NULL),
            /*C*/ NULL);
    }
    static ActionNode* backstab(PlayerbotAI* ai)
    {
        return new ActionNode("backstab",
            /*P*/ NULL,
            /*A*/ NextAction::array(0, new NextAction("cheap shot"), NULL),
            /*C*/ NULL);
    }
    ACTION_NODE_A(cheap_shot, "cheap shot", "sinister strike");
};

StealthedRogueStrategy::StealthedRogueStrategy(PlayerbotAI* ai) : Strategy(ai)
{
    actionNodeFactories.Add(new StealthedRogueStrategyActionNodeFactory());
}

void StealthedRogueStrategy::InitTriggers(std::list<TriggerNode*> &triggers)
{
    triggers.push_back(new TriggerNode(
        "behind target",
        NextAction::array(0, new NextAction("ambush", ACTION_HIGH + 3), NULL)));

    triggers.push_back(new TriggerNode(
        "not behind target",
        NextAction::array(0, new NextAction("cheap shot", ACTION_HIGH + 2), NULL)));

    triggers.push_back(new TriggerNode(
        "3 combo",
        NextAction::array(0, new NextAction("eviscerate", ACTION_HIGH), NULL)));

    triggers.push_back(new TriggerNode(
        "kick",
        NextAction::array(0, new NextAction("cheap shot", ACTION_HIGH), NULL)));

    triggers.push_back(new TriggerNode(
        "enemy flagcarrier near",
        NextAction::array(0, new NextAction("sprint", 81.0f), new NextAction("unstealth", 80.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "unstealth",
        NextAction::array(0, new NextAction("unstealth", ACTION_HIGH), NULL)));

    /*triggers.push_back(new TriggerNode(
        "low health",
        NextAction::array(0, new NextAction("food", ACTION_EMERGENCY + 1), NULL)));*/

    triggers.push_back(new TriggerNode(
        "no stealth",
        NextAction::array(0, new NextAction("check stealth", ACTION_HIGH + 10), NULL)));

    triggers.push_back(new TriggerNode(
        "sprint",
        NextAction::array(0, new NextAction("sprint", ACTION_HIGH + 10), NULL)));

    triggers.push_back(new TriggerNode(
        "enemy out of melee",
        NextAction::array(0, new NextAction("shadowstep", 62.0f), NULL)));
}

void StealthStrategy::InitTriggers(std::list<TriggerNode*> &triggers)
{
    triggers.push_back(new TriggerNode(
        "stealth",
        NextAction::array(0, new NextAction("stealth", ACTION_EMERGENCY), NULL)));
}

void RogueAoeStrategy::InitTriggers(std::list<TriggerNode*> &triggers)
{
    triggers.push_back(new TriggerNode(
        "light aoe",
        NextAction::array(0, new NextAction("blade flurry", ACTION_HIGH), NULL)));
}

void RogueBoostStrategy::InitTriggers(std::list<TriggerNode*> &triggers)
{
    triggers.push_back(new TriggerNode(
        "adrenaline rush",
        NextAction::array(0, new NextAction("adrenaline rush", ACTION_HIGH + 2), NULL)));

    triggers.push_back(new TriggerNode(
        "blade flurry",
        NextAction::array(0, new NextAction("blade flurry", ACTION_NORMAL + 9), NULL)));
}

class RogueCcStrategyActionNodeFactory : public NamedObjectFactory<ActionNode>
{
public:
    RogueCcStrategyActionNodeFactory()
    {
        creators["sap"] = &sap;
    }
private:
    ACTION_NODE_A(sap, "sap", "blind");
};

RogueCcStrategy::RogueCcStrategy(PlayerbotAI* ai) : Strategy(ai)
{
    actionNodeFactories.Add(new RogueCcStrategyActionNodeFactory());
}

void RogueCcStrategy::InitTriggers(std::list<TriggerNode*> &triggers)
{
    triggers.push_back(new TriggerNode(
        "sap",
        NextAction::array(0, new NextAction("sap", ACTION_INTERRUPT), NULL)));
}
