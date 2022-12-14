#include "botpch.h"
#include "../../playerbot.h"
#include "ResetAiAction.h"
#include "../../PlayerbotDbStore.h"

using namespace ai;

bool ResetAiAction::ExecuteCommand(Event& event)
{
    if (fullReset)
    {
        sPlayerbotDbStore.Reset(ai);
        ai->TellError("AI was reset to defaults");
    }
    ai->ResetStrategies(!fullReset);
    return true;
}
