#pragma once

#include "../Action.h"

namespace ai
{
    class XpGainAction : public Action
    {
    public:
        XpGainAction(PlayerbotAI* ai) : Action(ai, "xp gain") {}
        virtual bool Execute(Event& event);
    private:
        void GiveXP(uint32 xp, Unit* victim);
    };
}