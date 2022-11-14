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
        creators["pull sequence"] = &pull_sequence;
    }

private:
    static ActionNode* pull_sequence(PlayerbotAI* ai)
    {
        return new ActionNode("pull start",
            /*P*/ NULL,
            /*A*/ NULL,
            /*C*/ NextAction::array(0, new NextAction("pull action", 10.0f), NULL));
    }
};

PullStrategy::PullStrategy(PlayerbotAI* ai, string pullAction, float pullRange)
: Strategy(ai)
, action(pullAction)
, range(pullRange)
, started(false)
{
    actionNodeFactories.Add(new PullStrategyActionNodeFactory());
}

void PullStrategy::InitTriggers(std::list<TriggerNode*> &triggers)
{
    triggers.push_back(new TriggerNode(
        "pull start",
        NextAction::array(0, new NextAction("pull sequence", 60), NULL)));

    triggers.push_back(new TriggerNode(
        "pull end",
        NextAction::array(0, new NextAction("pull end", 60), NULL)));
}

void PullStrategy::InitMultipliers(std::list<Multiplier*> &multipliers)
{
    multipliers.push_back(new PullMultiplier(ai, action));
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

bool PullStrategy::CanPull(Unit* target) const
{
    bool canPull = false;

    // Check if the bot has the pull strategy enabled
    PullStrategy* pullStrategy = PullStrategy::Get(ai);
    if (pullStrategy)
    {
        // Check if the bot is currently pulling
        if (!pullStrategy->IsPulling())
        {
            // Check if the bot can perform the pull action
            const string& pullAction = GetAction();
            if (!pullAction.empty())
            {
                // Temporarily set the pull target to be used by the can do specific action method
                AiObjectContext* context = ai->GetAiObjectContext();
                SET_AI_VALUE(Unit*, "pull target", target);

                const string targetNameQualifier = "pull target";
                const string ignoreRangeQualifier = std::to_string(true);
                const vector<string> qualifiers = { targetNameQualifier, ignoreRangeQualifier };
                canPull = ai->CanDoSpecificAction(pullAction, Qualified::MultiQualify(qualifiers, ":"), true, false);

                // Clear the previously set pull target
                SET_AI_VALUE(Unit*, "pull target", nullptr);
            }
        }
    }

    return canPull;
}

string PullStrategy::GetAction() const
{
    string pullAction = action;

    // Select the faerie fire based on druid strategy
    if (ai->GetBot()->getClass() == CLASS_DRUID)
    {
        if (ai->HasStrategy("bear", BotState::BOT_STATE_COMBAT) || ai->HasStrategy("cat", BotState::BOT_STATE_COMBAT))
        {
            pullAction = "faerie fire (feral)";
        }
        else
        {
            pullAction = "faerie fire";
        }
    }

    return pullAction;
}

float PullMultiplier::GetValue(Action* action)
{
    const PullStrategy* strategy = PullStrategy::Get(ai);
    if (strategy && strategy->IsPulling())
    {
        if ((action->getName() == "pull start") ||
            (action->getName() == "pull action") ||
            (action->getName() == "pull end"))
        {
            return 1.0f;
        }

        return 0.0f;
    }

    return 1.0f;
}

void PossibleAdsStrategy::InitTriggers(std::list<TriggerNode*> &triggers)
{
    triggers.push_back(new TriggerNode(
        "possible ads",
        NextAction::array(0, new NextAction("flee with pet", 60), NULL)));
}