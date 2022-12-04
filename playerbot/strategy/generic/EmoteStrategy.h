#pragma once
#include "../Strategy.h"

namespace ai
{
    class EmoteStrategy : public Strategy
    {
    public:
        EmoteStrategy(PlayerbotAI* ai) : Strategy(ai) {}
        string getName() override { return "emote"; }
#ifndef GenerateBotHelp
        virtual string GetHelpName() { return "emote"; } //Must equal iternal name
        virtual string GetHelpDescription() {
            return "This strategy will make the bot react to or randomly send emotes.";
        }
        virtual vector<string> GetRelatedStrategies() { return { "rpg" }; }
#endif
    private:
        void InitNonCombatTriggers(std::list<TriggerNode*> &triggers) override;
    };
}
