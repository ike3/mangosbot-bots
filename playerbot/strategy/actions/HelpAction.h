#pragma once
#include "GenericActions.h"

namespace ai
{
    class HelpAction : public ChatCommandAction
    {
    public:
        HelpAction(PlayerbotAI* ai);
        virtual ~HelpAction();
        virtual bool ExecuteCommand(Event& event) override;

    private:
        void TellChatCommands();
        void TellStrategies();
        string CombineSupported(set<string> commands);

    private:
        NamedObjectContext<Action>* chatContext;
    };

}
