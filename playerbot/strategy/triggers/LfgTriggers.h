#pragma once

#include "../Trigger.h"

namespace ai
{
class LfgProposalActiveTrigger : public Trigger
{
public:
    LfgProposalActiveTrigger(PlayerbotAI* ai) : Trigger(ai, "lfg proposal active", 20) {}

    virtual bool IsActive();
};
}
