#include "botpch.h"
#include "../../playerbot.h"
#include "../generic/PullStrategy.h"
#include "../values/PossibleTargetsValue.h"
#include "PlayerbotAIConfig.h"
#include "PullActions.h"

using namespace ai;

bool PullMyTargetAction::Execute(Event& event)
{
    PullStrategy* strategy = PullStrategy::Get(ai);
    if (!strategy)
    {
        return false;
    }

    Unit* target = nullptr;
    if (event.getSource() == "attack anything")
    {
        ObjectGuid guid = event.getObject();
        target = ai->GetCreature(guid);
    }
    else if (Player* master = GetMaster())
    {
        target = ai->GetUnit(master->GetSelectionGuid());
    }

    if (!target)
    {
        ai->TellError("You have no target");
        return false;
    }

    const float maxPullDistance = sPlayerbotAIConfig.reactDistance * 3;
    const float distanceToPullTarget = target->GetDistance(ai->GetBot());
    if (distanceToPullTarget > maxPullDistance)
    {
        ai->TellError("The target is too far away");
        return false;
    }

    if (!PossibleTargetsValue::IsValid(bot, target, maxPullDistance))
    {
        ai->TellError("The target can't be pulled");
        return false;
    }

    if (!strategy->CanDoPullAction(target))
    {
        ostringstream out; out << "Can't perform pull action '" << strategy->GetActionName() << "'";
        ai->TellError(out.str());
        return false;
    }

    strategy->RequestPull(target);
    return true;
}

bool PullStartAction::Execute(Event& event)
{
    bool result = false;
    PullStrategy* strategy = PullStrategy::Get(ai);
    if (strategy)
    {
        Unit* target = strategy->GetTarget();
        if (target)
        {
            result = true;

            // Check if we are not on pull range
            const float distanceToTarget = target->GetDistance(bot);
            if (distanceToTarget > strategy->GetRange())
            {
                result = ai->DoSpecificAction("reach pull", event);
            }

            strategy->OnPullStarted();
        }
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