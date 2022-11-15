#include "botpch.h"
#include "../../playerbot.h"
#include "../generic/PullStrategy.h"
#include "../values/PossibleTargetsValue.h"
#include "PlayerbotAIConfig.h"
#include "PullActions.h"

using namespace ai;

bool PullMyTargetAction::Execute(Event& event)
{
    Player* master = GetMaster();
    if (master)
    {
        Unit* target = ai->GetUnit(master->GetSelectionGuid());
        if (target)
        {
            PullStrategy* strategy = PullStrategy::Get(ai);
            if (strategy)
            {
                strategy->RequestPull(target);
                return true;
            }
        }
    }

    return false;
}

bool PullMyTargetAction::isPossible()
{
    Player* master = GetMaster();
    if (master)
    {
        PullStrategy* strategy = PullStrategy::Get(ai);
        if (strategy)
        {
            Unit* target = ai->GetUnit(master->GetSelectionGuid());
            if (target)
            {
                const float maxPullDistance = sPlayerbotAIConfig.sightDistance;
                const float distanceToPullTarget = target->GetDistance(ai->GetBot());
                if (distanceToPullTarget <= maxPullDistance)
                {
                    if (PossibleTargetsValue::IsValid(bot, target, maxPullDistance))
                    {
                        if (strategy->CanDoPullAction(target))
                        {
                            return true;
                        }
                        else
                        {
                            ostringstream out; out << "Can't perform pull action '" << strategy->GetActionName() << "'";
                            ai->TellError(out.str());
                        }
                    }
                    else
                    {
                        ai->TellError("The target can't be pulled");
                    }
                }
                else
                {
                    ai->TellError("The target is too far away");
                }
            }
            else
            {
                ai->TellError("You have no target");
            }
        }
    }

    return false;
}

bool PullStartAction::Execute(Event& event)
{
    bool result = false;
    PullStrategy* strategy = PullStrategy::Get(ai);
    if (strategy)
    {
        result = ai->DoSpecificAction("reach pull", event);
        strategy->OnPullStarted();
    }

    return result;
}

bool PullAction::Execute(Event& event)
{
    PullStrategy* strategy = PullStrategy::Get(ai);
    if (strategy)
    {
        Unit* target = strategy->GetTarget();
        if (target)
        {
            // Check if we are on pull range
            const float distanceToTarget = target->GetDistance(bot);
            if (distanceToTarget <= strategy->GetRange())
            {
                // Force stop
                ai->StopMoving();

                // Execute the pull action
                const string targetNameQualifier = "pull target";
                const string ignoreRangeQualifier = std::to_string(false);
                const vector<string> qualifiers = { targetNameQualifier, ignoreRangeQualifier };
                return ai->DoSpecificAction(strategy->GetActionName(), event, false, Qualified::MultiQualify(qualifiers, ":"));
            }
            else
            {
                // Retry the reach pull action
                strategy->RequestPull(target);
            }
        }
    }

    return false;
}

bool PullEndAction::Execute(Event& event)
{
    PullStrategy* strategy = PullStrategy::Get(ai);
    if (strategy)
    {
        strategy->OnPullEnded();
        return true;
    }

    return false;
}