#pragma once

namespace ai
{
    class DebugStrategy : public Strategy
    {
    public:
        DebugStrategy(PlayerbotAI* ai) : Strategy(ai) {}
        virtual int GetType() { return STRATEGY_TYPE_NONCOMBAT; }
        virtual string getName() { return "debug"; }

#ifdef GenerateBotHelp
        virtual string GetHelpName() { return "debug"; } //Must equal iternal name
        virtual string GetHelpDescription() {
            return "This strategy will make the bot give chat feedback on the current actions (relevance) [triggers] it is trying to do.";
        }
        virtual vector<string> GetRelatedStrategies() { return { "debug action", "debug move" , "debug rpg", "debug spell",  "debug travel", "debug threat" }; }
#endif
    };
    class DebugActionStrategy : public DebugStrategy
    {
    public:
        DebugActionStrategy(PlayerbotAI* ai) : DebugStrategy(ai) {}
        virtual int GetType() { return STRATEGY_TYPE_NONCOMBAT; }
        virtual string getName() { return "debug action"; }

#ifdef GenerateBotHelp
        virtual string GetHelpName() { return "debug action"; } //Must equal iternal name
        virtual string GetHelpDescription() {
            return "This strategy will make the bot give chat feedback on the current actions the bot is considering to do but are impossible or not usefull.";
        }
        virtual vector<string> GetRelatedStrategies() { return { "debug"}; }
#endif
    };
    class DebugMoveStrategy : public Strategy
    {
    public:
        DebugMoveStrategy(PlayerbotAI* ai) : Strategy(ai) {}
        virtual int GetType() { return STRATEGY_TYPE_NONCOMBAT; }
        virtual string getName() { return "debug move"; }
#ifdef GenerateBotHelp
        virtual string GetHelpName() { return "debug move"; } //Must equal iternal name
        virtual string GetHelpDescription() {
            return "This strategy will make the bot give chat and visual feedback for it's current movement actions.";
        }
        virtual vector<string> GetRelatedStrategies() { return { "debug", "rpg", "travel", "follow"}; }
#endif
    };
    class DebugRpgStrategy : public Strategy
    {
    public:
        DebugRpgStrategy(PlayerbotAI* ai) : Strategy(ai) {}
        virtual int GetType() { return STRATEGY_TYPE_NONCOMBAT; }
        virtual string getName() { return "debug rpg"; }
#ifdef GenerateBotHelp
        virtual string GetHelpName() { return "debug rpg"; } //Must equal iternal name
        virtual string GetHelpDescription() {
            return "This strategy will make the bot give chat feedback on rpg target selection during [h:action|choose rpg target] and  [h:action|move to rpg target].";
        }
        virtual vector<string> GetRelatedStrategies() { return { "debug", "rpg"}; }
#endif
    };

    class DebugSpellStrategy : public Strategy
    {
    public:
        DebugSpellStrategy(PlayerbotAI* ai) : Strategy(ai) {}
        virtual int GetType() { return STRATEGY_TYPE_NONCOMBAT; }
        virtual string getName() { return "debug spell"; }
#ifdef GenerateBotHelp
        virtual string GetHelpName() { return "debug spell"; } //Must equal iternal name
        virtual string GetHelpDescription() {
            return "This strategy will make the bot give chat feedback on the current spell it is casting.";
        }
        virtual vector<string> GetRelatedStrategies() { return { "debug"}; }
#endif
    };

    class DebugTravelStrategy : public Strategy
    {
    public:
        DebugTravelStrategy(PlayerbotAI* ai) : Strategy(ai) {}
        virtual int GetType() { return STRATEGY_TYPE_NONCOMBAT; }
        virtual string getName() { return "debug travel"; }
#ifdef GenerateBotHelp
        virtual string GetHelpName() { return "debug travel"; } //Must equal iternal name
        virtual string GetHelpDescription() {
            return "This strategy will make the bot give chat feedback on the locations it is considering during [h:action|choose travel target].";
        }
        virtual vector<string> GetRelatedStrategies() { return { "debug" , "travel"}; }
#endif
    };

    class DebugThreatStrategy : public Strategy
    {
    public:
        DebugThreatStrategy(PlayerbotAI* ai) : Strategy(ai) {}
        virtual int GetType() { return STRATEGY_TYPE_NONCOMBAT; }
        virtual string getName() { return "debug threat"; }
#ifdef GenerateBotHelp
        virtual string GetHelpName() { return "debug threat"; } //Must equal iternal name
        virtual string GetHelpDescription() {
            return "This strategy will make the bot give chat (noncombat) or visual (combat) feedback on it's current [h:value:threat|threat value].";
        }
        virtual vector<string> GetRelatedStrategies() { return { "debug" , "threat" }; }
#endif
    };

    class DebugMountStrategy : public Strategy
    {
    public:
        DebugMountStrategy(PlayerbotAI* ai) : Strategy(ai) {}
        virtual int GetType() { return STRATEGY_TYPE_NONCOMBAT; }
        virtual string getName() { return "debug mount"; }
#ifdef GenerateBotHelp
        virtual string GetHelpName() { return "debug mount"; } //Must equal iternal name
        virtual string GetHelpDescription() {
            return "This strategy will make the bot give chat feedback during mount actions.";
        }
        virtual vector<string> GetRelatedStrategies() { return { "debug" , "threat" }; }
#endif
    };
    
    class DebugGrindStrategy : public Strategy
    {
    public:
        DebugGrindStrategy(PlayerbotAI* ai) : Strategy(ai) {}
        virtual int GetType() { return STRATEGY_TYPE_NONCOMBAT; }
        virtual string getName() { return "debug grind"; }
#ifdef GenerateBotHelp
        virtual string GetHelpName() { return "debug grind"; } //Must equal iternal name
        virtual string GetHelpDescription() {
            return "This strategy will make the bot give chat feedback about grind target selection.";
        }
        virtual vector<string> GetRelatedStrategies() { return { "debug" , "threat" }; }
#endif
    };
}
