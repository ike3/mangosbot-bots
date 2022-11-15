#include "botpch.h"
#include "../../playerbot.h"
#include "../Action.h"
#include "../generic/PullStrategy.h"
#include "PullTriggers.h"

using namespace ai;

bool PullStartTrigger::IsActive()
{
    const PullStrategy* strategy = PullStrategy::Get(ai);
    return strategy && strategy->IsPullPendingToStart();
}

bool PullEndTrigger::IsActive()
{
    const PullStrategy* strategy = PullStrategy::Get(ai);
    if (strategy && strategy->HasPullStarted())
    {
        Unit* target = strategy->GetTarget();
        if (target)
        {
            // Check if the pull is taking too long
            const time_t secondsSincePullStarted = time(0) - strategy->GetPullStartTime();
            if (secondsSincePullStarted >= strategy->GetMaxPullTime())
            {
                return true;
            }
            else
            {
                // Check if the pulled target has approached the bot
                const float distanceToPullTarget = target->GetDistance(ai->GetBot());
                if (distanceToPullTarget <= ATTACK_DISTANCE)
                {
                    return true;
                }
            }
        }
    }

    return false;
}
