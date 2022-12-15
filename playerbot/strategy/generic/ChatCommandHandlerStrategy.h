#pragma once
#include "PassTroughStrategy.h"

namespace ai
{
    class ChatCommandHandlerStrategy : public PassTroughStrategy
    {
    public:
        ChatCommandHandlerStrategy(PlayerbotAI* ai);
        string getName() override { return "chat"; }

#ifdef GenerateBotHelp
        virtual string GetHelpName() { return "chat"; } //Must equal internal name
        virtual string GetHelpDescription() 
        {
            return "This strategy will make bots respond to various chat commands.";
        }
        virtual vector<string> GetRelatedStrategies() { return { }; }
#endif

    private:
        void InitReactionTriggers(std::list<TriggerNode*> &triggers) override;
    };
}
