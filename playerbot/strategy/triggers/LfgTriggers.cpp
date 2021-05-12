#include "botpch.h"
#include "../../playerbot.h"
#include "LfgTriggers.h"

using namespace ai;

bool LfgProposalActiveTrigger::IsActive()
{
    return AI_VALUE(uint32, "lfg proposal");
}
