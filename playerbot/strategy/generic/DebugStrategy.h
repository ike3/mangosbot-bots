#pragma once

namespace ai
{
    class DebugStrategy : public Strategy
    {
    public:
        DebugStrategy(PlayerbotAI* ai) : Strategy(ai) {}
        virtual int GetType() { return STRATEGY_TYPE_NONCOMBAT; }
        virtual string getName() { return "debug"; }
    };
    class DebugMoveStrategy : public Strategy
    {
    public:
        DebugMoveStrategy(PlayerbotAI* ai) : Strategy(ai) {}
        virtual int GetType() { return STRATEGY_TYPE_NONCOMBAT; }
        virtual string getName() { return "debug move"; }
    };
    class DebugRpgStrategy : public Strategy
    {
    public:
        DebugRpgStrategy(PlayerbotAI* ai) : Strategy(ai) {}
        virtual int GetType() { return STRATEGY_TYPE_NONCOMBAT; }
        virtual string getName() { return "debug rpg"; }
    };

    class DebugSpellStrategy : public Strategy
    {
    public:
        DebugSpellStrategy(PlayerbotAI* ai) : Strategy(ai) {}
        virtual int GetType() { return STRATEGY_TYPE_NONCOMBAT; }
        virtual string getName() { return "debug spell"; }
    };
}
