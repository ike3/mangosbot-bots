#pragma once
#include "PassTroughStrategy.h"

namespace ai
{
    class ChatCommandHandlerStrategy : public PassTroughStrategy
    {
    public:
        ChatCommandHandlerStrategy(PlayerbotAI* ai);
        string getName() override { return "chat"; }
#ifndef GenerateBotHelp
        virtual string GetHelpName() { return "chat"; } //Must equal iternal name
        virtual string GetHelpDescription() {
            return "This strategy will make bots respond to various chat commands.";
        }
        virtual vector<string> GetRelatedStrategies() { return { }; }
#endif
    private:
        void InitNonCombatTriggers(std::list<TriggerNode*> &triggers) override;
        void InitCombatTriggers(std::list<TriggerNode*>& triggers) override;
        void InitDeadTriggers(std::list<TriggerNode*>& triggers) override;
    };
}
