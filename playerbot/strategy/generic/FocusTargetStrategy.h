#pragma once
#include "../Strategy.h"

namespace ai
{
    class FocusHealTargetStrategy : public Strategy
    {
    public:
        FocusHealTargetStrategy(PlayerbotAI* ai) : Strategy(ai) {}
        string getName() override { return "focus heal target"; }

#ifdef GenerateBotHelp
        virtual string GetHelpName() { return "focus heal target"; } //Must equal iternal name
        virtual string GetHelpDescription()
        {
            return "This strategy will make the bot focus heal the specified target using the 'set focus heal <targetname>' command";
        }
#endif
    };
}