#pragma once
#include "GenericActions.h"

namespace ai
{
    class ResetAiAction : public ChatCommandAction
    {
    public:
        ResetAiAction(PlayerbotAI* ai, bool fullReset = true, string name = "reset ai") : ChatCommandAction(ai, name), fullReset(fullReset) {}
        virtual bool Execute(Event& event) override;

#ifdef GenerateBotHelp
        virtual string GetHelpName() { return "reset ai"; } //Must equal iternal name
        virtual string GetHelpDescription()
        {
            return "Reset the bot to it's initial state.\n"
                "Saved settings and values will be cleared.";
        }
        virtual vector<string> GetUsedActions() { return { "reset strats" , "reset values" }; }
        virtual vector<string> GetUsedValues() { return {}; }
#endif 
    protected:
        virtual void ResetStrategies();
        virtual void ResetValues();
        bool fullReset;
    };

    class ResetStratsAction : public ResetAiAction
    {
    public:
        ResetStratsAction(PlayerbotAI* ai, string name = "reset strats", bool fullReset = true) : ResetAiAction(ai, fullReset, name) {}
        virtual bool Execute(Event& event) override;

#ifdef GenerateBotHelp
        virtual string GetHelpName() { return "reset strats"; } //Must equal iternal name
        virtual string GetHelpDescription()
        {
            return "Reset the strategies of the bot to the standard values.\n"
                "Saved strategies will be cleared.";
        }
        virtual vector<string> GetUsedActions() { return {"reset ai"}; }
        virtual vector<string> GetUsedValues() { return {}; }
#endif 
    };

    class ResetValuesAction : public ResetAiAction
    {
    public:
        ResetValuesAction(PlayerbotAI* ai, string name = "reset values", bool fullReset = true) : ResetAiAction(ai, fullReset, name) {}
        virtual bool Execute(Event& event) override;

#ifdef GenerateBotHelp
        virtual string GetHelpName() { return "reset values"; } //Must equal iternal name
        virtual string GetHelpDescription()
        {
            return "Reset the settings of the bot to the default values.\n"
                "Saved values will be cleared.";
        }
        virtual vector<string> GetUsedActions() { return { "reset ai" }; }
        virtual vector<string> GetUsedValues() { return {}; }
#endif 
    };

    class ResetAction : public Action
    {
    public:
        ResetAction(PlayerbotAI* ai, string name = "reset") : Action(ai, name) {}
        virtual bool Execute(Event& event) override { ai->Reset(true); return true; };

#ifdef GenerateBotHelp
        virtual string GetHelpName() { return "reset"; } //Must equal iternal name
        virtual string GetHelpDescription()
        {
            return "Reset internal buffers to clear current behavior.";
        }
        virtual vector<string> GetUsedActions() { return { "reset ai" }; }
        virtual vector<string> GetUsedValues() { return {}; }
#endif 
    };
}
