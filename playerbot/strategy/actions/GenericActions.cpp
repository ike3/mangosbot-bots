#include "botpch.h"
#include "../../playerbot.h"
#include "GenericActions.h"

using namespace ai;

bool MeleeAction::isUseful()
{
    // do not allow if can't attack from vehicle
    if (ai->IsInVehicle() && !ai->IsInVehicle(false, false, true))
        return false;

    return true;
}

bool UpdateStrategyDependenciesAction::Execute(Event& event)
{
    if (!strategiesToAdd.empty() || !strategiesToRemove.empty())
    {
        // Strategies to add
        for (const StrategyToUpdate* strategy : strategiesToAdd)
        {
            std::stringstream changeStr;
            changeStr << "+" << strategy->name;
            ai->ChangeStrategy(changeStr.str(), strategy->state);
        }

        // Strategies to remove
        for (const StrategyToUpdate* strategy : strategiesToRemove)
        {
            std::stringstream changeStr;
            changeStr << "-" << strategy->name;
            ai->ChangeStrategy(changeStr.str(), strategy->state);
        }

        return true;
    }

    return false;
}

bool UpdateStrategyDependenciesAction::isUseful()
{
    if (!strategiesToUpdate.empty())
    {
        strategiesToAdd.clear();
        strategiesToRemove.clear();
        for (const StrategyToUpdate& strategy : strategiesToUpdate)
        {
            // Ignore if the strategies required are not found
            bool requiredStrategyMissing = false;
            for (const std::string strategyRequired : strategy.strategiesRequired)
            {
                if (!ai->HasStrategy(strategyRequired, strategy.state))
                {
                    requiredStrategyMissing = true;
                    break;
                }
            }

            if (requiredStrategyMissing)
            {
                // Check if we need to remove the strategy
                if (ai->HasStrategy(strategy.name, strategy.state))
                {
                    strategiesToRemove.emplace_back(&strategy);
                }
            }
            else
            {
                // Check if we need to add the strategy
                if (!ai->HasStrategy(strategy.name, strategy.state))
                {
                    strategiesToAdd.emplace_back(&strategy);
                }
            }
        }
    }

    return !strategiesToAdd.empty() || !strategiesToRemove.empty();
}