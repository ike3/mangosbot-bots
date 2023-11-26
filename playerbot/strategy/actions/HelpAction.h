#pragma once
#include "GenericActions.h"

namespace ai
{
    class HelpAction : public ChatCommandAction
    {
    public:
        HelpAction(PlayerbotAI* ai);
        virtual ~HelpAction();
        virtual bool Execute(Event& event) override;

    private:
        void TellChatCommands(Player* requester);
        void TellStrategies(Player* requester);
        string CombineSupported(set<string> commands);

#ifdef GenerateBotHelp
        virtual string GetHelpName() { return "help"; } //Must equal iternal name
        virtual string GetHelpDescription()
        {
            return "This chat command will make the bot give you help on various topics.\n"
                "Each help topic will provide various chat-links which can be linked in chat.\n"
                "Links can be copied to chat with shift-click and when sent to a bot will provide feedback.\n"
                "It is possible to search for help topics by adding part of the topic name after the command.\n"
                "Example commands: 'help' and 'help rpg'.";
        }
        virtual vector<string> GetUsedActions() { return {}; }
        virtual vector<string> GetUsedValues() { return {}; }
#endif 
    private:
        NamedObjectContext<Action>* chatContext;
    };

}
