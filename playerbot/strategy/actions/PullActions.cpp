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
            if (PossibleTargetsValue::IsValid(bot, target, sPlayerbotAIConfig.sightDistance))
            {
                PullStrategy* strategy = PullStrategy::Get(ai);
                if (strategy)
                {
                    strategy->SetTarget(target);
                    return true;
                }
            }
            else if (verbose)
            {
                ai->TellError("The target can't be pulled");
            }
        }
        else if (verbose)
        {
            ai->TellError("You have no target");
        }
    }

    return false;
}

bool PullMyTargetAction::isPossible()
{
    Player* master = GetMaster();
    if (master)
    {
        Unit* target = ai->GetUnit(master->GetSelectionGuid());
        if (target)
        {
            if (PossibleTargetsValue::IsValid(bot, target, sPlayerbotAIConfig.sightDistance))
            {
                PullStrategy* strategy = PullStrategy::Get(ai);
                return strategy && strategy->CanPull(target);
            }
            else if (verbose)
            {
                ai->TellError("The target can't be pulled");
            }
        }
        else if (verbose)
        {
            ai->TellError("You have no target");
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
        strategy->OnPullStart();
    }

    return result;
}

bool PullAction::Execute(Event& event)
{
    PullStrategy* strategy = PullStrategy::Get(ai);
    if (strategy)
    {
        // Sometimes the reach pull action finishes before it has reached the pull range
        ai->StopMoving();

        // Execute the pull action
        const string targetNameQualifier = "pull target";
        const string ignoreRangeQualifier = std::to_string(false);
        const vector<string> qualifiers = { targetNameQualifier, ignoreRangeQualifier };
        return ai->DoSpecificAction(strategy->GetAction(), event, false, Qualified::MultiQualify(qualifiers, ":"));
    }

    return false;
}

bool PullEndAction::Execute(Event& event)
{
    bool result = false;
    PullStrategy* strategy = PullStrategy::Get(ai);
    if (strategy)
    {
        strategy->SetTarget(nullptr);
        strategy->OnPullEnd();
        result = true;
    }

    return result;
}