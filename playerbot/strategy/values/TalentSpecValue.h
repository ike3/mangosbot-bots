#pragma once
#include "../Value.h"
#include "../../PlayerTalentSpec.h"

namespace ai
{
    class TalentSpecValue : public ManualSetValue<PlayerTalentSpec>
    {
    public:
        TalentSpecValue(PlayerbotAI* ai, string name = "talent spec") : ManualSetValue<PlayerTalentSpec>(ai, PlayerTalentSpec::TALENT_SPEC_INVALID, name) {}
#ifdef GenerateBotHelp
        virtual string GetHelpName() { return "talent spec"; } //Must equal iternal name
        virtual string GetHelpTypeName() { return "talent spec"; }
        virtual string GetHelpDescription() { return "This value stores the current talent spec for the bot."; }
        virtual vector<string> GetUsedValues() { return {}; }
#endif 
    };
}
