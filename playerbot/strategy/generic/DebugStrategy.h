#pragma once

namespace ai
{
    class DebugStrategy : public Strategy
    {
    public:
        DebugStrategy(PlayerbotAI* ai) : Strategy(ai) {}
        virtual int GetType() { return STRATEGY_TYPE_NONCOMBAT; }
        virtual string getName() { return "debug"; }

#ifndef GenerateBotHelp
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

#ifndef GenerateBotHelp
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
#ifndef GenerateBotHelp
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
#ifndef GenerateBotHelp
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
#ifndef GenerateBotHelp
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
#ifndef GenerateBotHelp
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
#ifndef GenerateBotHelp
        virtual string GetHelpName() { return "debug threat"; } //Must equal iternal name
        virtual string GetHelpDescription() {
            return "This strategy will make the bot give chat (noncombat) or visual (combat) feedback on it's current [h:value:threat|threat value].";
        }
        virtual vector<string> GetRelatedStrategies() { return { "debug" , "threat" }; }
#endif
    };
}
