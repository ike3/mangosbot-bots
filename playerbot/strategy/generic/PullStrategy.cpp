#include "botpch.h"
#include "../../playerbot.h"
#include "../PassiveMultiplier.h"
#include "PullStrategy.h"

using namespace ai;

class PullStrategyActionNodeFactory : public NamedObjectFactory<ActionNode>
{
public:
    PullStrategyActionNodeFactory()
    {
        creators["pull start"] = &pull_start;
    }

private:
    static ActionNode* pull_start(PlayerbotAI* ai)
    {
        return new ActionNode("pull start",
            /*P*/ NULL,
            /*A*/ NULL,
            /*C*/ NextAction::array(0, new NextAction("pull action", 10.0f), NULL));
    }
};

string GetSpellName(PlayerbotAI* ai, const string& actionName)
{
    string spellName = actionName;
    if (spellName == "shoot")
    {
        const Item* equippedWeapon = ai->GetBot()->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_RANGED);
        if (equippedWeapon)
        {
            const ItemPrototype* itemPrototype = equippedWeapon->GetProto();
            if (itemPrototype)
            {
                switch (itemPrototype->SubClass)
                {
                    case ITEM_SUBCLASS_WEAPON_GUN:
                    {
#ifdef MANGOSBOT_ZERO
                        spellName += " gun";
#endif
                        break;
                    }
                    case ITEM_SUBCLASS_WEAPON_BOW:
                    {
#ifdef MANGOSBOT_ZERO
                        spellName += " bow";
#endif
                        break;
                    }
                    case ITEM_SUBCLASS_WEAPON_CROSSBOW:
                    {
#ifdef MANGOSBOT_ZERO
                        spellName += " crossbow";
#endif
                        break;
                    }
                    case ITEM_SUBCLASS_WEAPON_THROWN:
                    {
                        spellName = "throw";

                        break;
                    }

                    default: break;
                }
            }
        }
    }

    return spellName;
}

PullStrategy::PullStrategy(PlayerbotAI* ai, string pullAction, string prePullAction)
    : Strategy(ai)
    , actionName(pullAction)
    , pendingToStart(false)
    , pullStartTime(0)
{
    actionNodeFactories.Add(new PullStrategyActionNodeFactory());

    if (!ai->GetBot())
        return;

    // Try to get the correct spell name
    string spellName = GetSpellName(ai, pullAction);

    // Try to get the pull action range
    if (ai->GetSpellRange(spellName, &range))
    {
        range -= CONTACT_DISTANCE;
    }
    else
    {
        // Set the default range if the range was not found
        range = (actionName == "shoot") ? ai->GetRange("shoot") : ai->GetRange("spell");
    }

    actionName = spellName;
    preActionName = prePullAction;
}

void PullStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode(
        "pull start",
        NextAction::array(0, new NextAction("pull start", 60), new NextAction("pull action", 60), NULL)));

    triggers.push_back(new TriggerNode(
        "pull end",
        NextAction::array(0, new NextAction("pull end", 60), NULL)));
}

void PullStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{
    InitCombatTriggers(triggers);
}

void PullStrategy::InitCombatMultipliers(std::list<Multiplier*>& multipliers)
{
    multipliers.push_back(new PullMultiplier(ai));
}

void PullStrategy::InitNonCombatMultipliers(std::list<Multiplier*>& multipliers)
{
    InitCombatMultipliers(multipliers);
}

PullStrategy* PullStrategy::Get(PlayerbotAI* ai)
{
    return ai ? ai->GetStrategy<PullStrategy>("pull", BotState::BOT_STATE_COMBAT) : nullptr;
}

Unit* PullStrategy::GetTarget() const
{
    AiObjectContext* context = ai->GetAiObjectContext();
    return AI_VALUE(Unit*, "pull target");
}

void PullStrategy::SetTarget(Unit* target)
{
    AiObjectContext* context = ai->GetAiObjectContext();
    SET_AI_VALUE(Unit*, "pull target", target);
}

bool PullStrategy::CanDoPullAction(Unit* target)
{
    // Check if the bot can perform the pull action
    bool canPull = false;
    const string& pullAction = GetActionName();
    if (!pullAction.empty())
    {
        // Temporarily set the pull target to be used by the can do specific action method
        AiObjectContext* context = ai->GetAiObjectContext();
        Unit* previousTarget = GetTarget();
        SetTarget(target);

        canPull = ai->CanDoSpecificAction("pull action", true, false);

        // Restore the previous pull target
        SetTarget(previousTarget);
    }

    return canPull;
}

string PullStrategy::GetActionName() const
{
    string pullAction = actionName;

    // Select the faerie fire based on druid strategy
    if (pullAction == "faerie fire")
    {
        if (ai->GetBot()->getClass() == CLASS_DRUID)
        {
            if (ai->HasAnyAuraOf(ai->GetBot(), "bear form", "dire bear form", "cat form", NULL))
            {
                pullAction = "faerie fire (feral)";
            }
        }
    }

    return pullAction;
}

void PullStrategy::OnPullStarted()
{
    pendingToStart = false;
}

void PullStrategy::OnPullEnded()
{
    pullStartTime = 0;
    SetTarget(nullptr);
}

void PullStrategy::RequestPull(Unit* target, bool resetTime)
{
    SetTarget(target);
    pendingToStart = true;
    pullStartTime = time(0);
}

float PullMultiplier::GetValue(Action* action)
{
    const PullStrategy* strategy = PullStrategy::Get(ai);
    if (strategy && strategy->HasTarget())
    {
        if ((action->getName() == "pull my target") ||
            (action->getName() == "reach pull") ||
            (action->getName() == "pull start") ||
            (action->getName() == "pull action") ||
            (action->getName() == "return to pull position") ||
            (action->getName() == "pull end"))
        {
            return 1.0f;
        }

        return 0.0f;
    }

    return 1.0f;
}

void PossibleAdsStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode(
        "possible ads",
        NextAction::array(0, new NextAction("flee with pet", 60), NULL)));
}

void PullBackStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode(
        "return to pull position",
        NextAction::array(0, new NextAction("return to pull position", 65.0f), NULL)));
}

void PullBackStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{
    InitCombatTriggers(triggers);
}