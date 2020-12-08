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
        creators["sinister strike"] = &sinister_strike;
        creators["kick"] = &kick;
        creators["kidney shot"] = &kidney_shot;
        creators["rupture"] = &rupture;
        creators["backstab"] = &backstab;
    }
private:
    static ActionNode* riposte(PlayerbotAI* ai)
    {
        return new ActionNode ("riposte",
            /*P*/ NULL,
            /*A*/ NextAction::array(0, new NextAction("mutilate"), NULL),
            /*C*/ NULL);
    }
    static ActionNode* mutilate(PlayerbotAI* ai)
    {
        return new ActionNode ("mutilate",
            /*P*/ NULL,
            /*A*/ NextAction::array(0, new NextAction("sinister strike"), NULL),
            /*C*/ NULL);
    }
    static ActionNode* sinister_strike(PlayerbotAI* ai)
    {
        return new ActionNode ("sinister strike",
            /*P*/ NULL,
            /*A*/ NextAction::array(0, new NextAction("melee"), NULL),
            /*C*/ NULL);
    }
    static ActionNode* kick(PlayerbotAI* ai)
    {
        return new ActionNode ("kick",
            /*P*/ NULL,
            /*A*/ NextAction::array(0, new NextAction("kidney shot"), NULL),
            /*C*/ NULL);
    }
    static ActionNode* kidney_shot(PlayerbotAI* ai)
    {
        return new ActionNode ("kidney shot",
            /*P*/ NULL,
            /*A*/ NULL,
            /*C*/ NULL);
    }
    static ActionNode* rupture(PlayerbotAI* ai)
    {
        return new ActionNode ("rupture",
            /*P*/ NULL,
            /*A*/ NextAction::array(0, new NextAction("eviscerate"), NULL),
            /*C*/ NULL);
    }
    static ActionNode* backstab(PlayerbotAI* ai)
    {
        return new ActionNode ("backstab",
            /*P*/ NULL,
            /*A*/ NULL,
            /*C*/ NULL);
    }
};

DpsRogueStrategy::DpsRogueStrategy(PlayerbotAI* ai) : CombatStrategy(ai)
{
    actionNodeFactories.Add(new DpsRogueStrategyActionNodeFactory());
}

NextAction** DpsRogueStrategy::getDefaultActions()
{
    return NextAction::array(0, new NextAction("riposte", ACTION_NORMAL), NULL);
}

void DpsRogueStrategy::InitTriggers(std::list<TriggerNode*> &triggers)
{
    CombatStrategy::InitTriggers(triggers);

    triggers.push_back(new TriggerNode(
        "combo points available",
        NextAction::array(0, new NextAction("rupture", ACTION_HIGH + 2), NULL)));

	triggers.push_back(new TriggerNode(
		"medium threat",
		NextAction::array(0, new NextAction("feint", ACTION_HIGH), NULL)));

	triggers.push_back(new TriggerNode(
		"low health",
		NextAction::array(0, new NextAction("evasion", ACTION_EMERGENCY), new NextAction("feint", ACTION_EMERGENCY), NULL)));

    triggers.push_back(new TriggerNode(
        "critical health",
        NextAction::array(0, new NextAction("blind", ACTION_EMERGENCY), new NextAction("vanish", ACTION_EMERGENCY), NULL)));

	triggers.push_back(new TriggerNode(
		"kick",
		NextAction::array(0, new NextAction("kick", ACTION_INTERRUPT + 2), NULL)));

	triggers.push_back(new TriggerNode(
		"kick on enemy healer",
		NextAction::array(0, new NextAction("kick on enemy healer", ACTION_INTERRUPT + 1), NULL)));

    triggers.push_back(new TriggerNode(
        "behind target",
        NextAction::array(0, new NextAction("backstab", ACTION_HIGH + 1), NULL)));

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
        NextAction::array(0, new NextAction("unstealth", ACTION_NORMAL), NULL)));

    triggers.push_back(new TriggerNode(
        "sprint",
        NextAction::array(0, new NextAction("sprint", ACTION_INTERRUPT), NULL)));
}

class StealthedRogueStrategyActionNodeFactory : public NamedObjectFactory<ActionNode>
{
public:
    StealthedRogueStrategyActionNodeFactory()
    {
        creators["ambush"] = &ambush;
        creators["cheap shot"] = &cheap_shot;
        creators["garrote"] = &garrote;
        creators["sap"] = &sap;
        creators["sinister strike"] = &sinister_strike;
    }
private:
    static ActionNode* ambush(PlayerbotAI* ai)
    {
        return new ActionNode("ambush",
            /*P*/ NULL,
            /*A*/ NextAction::array(0, new NextAction("garrote"), NULL),
            /*C*/ NULL);
    }
    static ActionNode* cheap_shot(PlayerbotAI* ai)
    {
        return new ActionNode("cheap shot",
            /*P*/ NULL,
            /*A*/ NULL,
            /*C*/ NULL);
    }
    static ActionNode* garrote(PlayerbotAI* ai)
    {
        return new ActionNode("garrote",
            /*P*/ NULL,
            /*A*/ NULL,
            /*C*/ NULL);
    }
    static ActionNode* sap(PlayerbotAI* ai)
    {
        return new ActionNode("sap",
            /*P*/ NULL,
            /*A*/ NULL,
            /*C*/ NULL);
    }
    static ActionNode* sinister_strike(PlayerbotAI* ai)
    {
        return new ActionNode("sinister strike",
            /*P*/ NULL,
            /*A*/ NextAction::array(0, new NextAction("cheap shot"), NULL),
            /*C*/ NULL);
    }
};

StealthedRogueStrategy::StealthedRogueStrategy(PlayerbotAI* ai) : Strategy(ai)
{
    actionNodeFactories.Add(new StealthedRogueStrategyActionNodeFactory());
}

NextAction** StealthedRogueStrategy::getDefaultActions()
{
    return NextAction::array(0,
        new NextAction("ambush", ACTION_NORMAL + 4),
        new NextAction("backstab", ACTION_NORMAL + 3),
        new NextAction("cheap shot", ACTION_NORMAL + 2),
        new NextAction("sinister strike", ACTION_NORMAL + 1),
        new NextAction("melee", ACTION_NORMAL),
        NULL);
}

void StealthedRogueStrategy::InitTriggers(std::list<TriggerNode*> &triggers)
{
    triggers.push_back(new TriggerNode(
        "combo points available",
        NextAction::array(0, new NextAction("eviscerate", ACTION_HIGH), NULL)));

    triggers.push_back(new TriggerNode(
        "kick",
        NextAction::array(0, new NextAction("cheap shot", ACTION_INTERRUPT), NULL)));

    triggers.push_back(new TriggerNode(
        "kick on enemy healer",
        NextAction::array(0, new NextAction("cheap shot", ACTION_INTERRUPT), NULL)));

    triggers.push_back(new TriggerNode(
        "behind target",
        NextAction::array(0, new NextAction("ambush", ACTION_HIGH), NULL)));

    triggers.push_back(new TriggerNode(
        "not behind target",
        NextAction::array(0, new NextAction("cheap shot", ACTION_HIGH), NULL)));

    triggers.push_back(new TriggerNode(
        "enemy flagcarrier near",
        NextAction::array(0, new NextAction("sprint", ACTION_EMERGENCY + 1), NULL)));

    triggers.push_back(new TriggerNode(
        "unstealth",
        NextAction::array(0, new NextAction("unstealth", ACTION_NORMAL), NULL)));

    /*triggers.push_back(new TriggerNode(
        "low health",
        NextAction::array(0, new NextAction("food", ACTION_EMERGENCY + 1), NULL)));*/

    triggers.push_back(new TriggerNode(
        "no stealth",
        NextAction::array(0, new NextAction("check stealth", ACTION_EMERGENCY), NULL)));

    triggers.push_back(new TriggerNode(
        "sprint",
        NextAction::array(0, new NextAction("sprint", ACTION_INTERRUPT), NULL)));
}

void StealthStrategy::InitTriggers(std::list<TriggerNode*> &triggers)
{
    triggers.push_back(new TriggerNode(
        "stealth",
        NextAction::array(0, new NextAction("stealth", ACTION_INTERRUPT), NULL)));
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
}

void RogueCcStrategy::InitTriggers(std::list<TriggerNode*> &triggers)
{
    triggers.push_back(new TriggerNode(
        "sap",
        NextAction::array(0, new NextAction("stealth", ACTION_INTERRUPT), new NextAction("sap", ACTION_INTERRUPT), NULL)));
}
