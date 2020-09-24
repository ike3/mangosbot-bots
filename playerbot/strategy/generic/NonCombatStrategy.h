#pragma once

namespace ai
{
    class NonCombatStrategy : public Strategy
    {
    public:
        NonCombatStrategy(PlayerbotAI* ai) : Strategy(ai) {}
		virtual int GetType() { return STRATEGY_TYPE_NONCOMBAT; }
        virtual void InitTriggers(std::list<TriggerNode*> &triggers);
    };

    class LfgStrategy : public Strategy
    {
    public:
        LfgStrategy(PlayerbotAI* ai) : Strategy(ai) {}
		virtual int GetType() { return STRATEGY_TYPE_NONCOMBAT; }
        virtual void InitTriggers(std::list<TriggerNode*> &triggers);
        virtual string getName() { return "lfg"; }
    };

    class CollisionStrategy : public Strategy
    {
    public:
        CollisionStrategy(PlayerbotAI* ai) : Strategy(ai) {}
		virtual int GetType() { return STRATEGY_TYPE_NONCOMBAT; }
        virtual void InitTriggers(std::list<TriggerNode*> &triggers);
        virtual string getName() { return "collision"; }
    };

    class BGStrategy : public Strategy
    {
    public:
        BGStrategy(PlayerbotAI* ai) : Strategy(ai) {};
        virtual int GetType() { return STRATEGY_TYPE_NONCOMBAT; }
        virtual void InitTriggers(std::list<TriggerNode*> &triggers);
        virtual string getName() { return "bg"; }
    };

    class WarsongStrategy : public Strategy
    {
    public:
        WarsongStrategy(PlayerbotAI* ai) : Strategy(ai) {};
        virtual int GetType() { return STRATEGY_TYPE_NONCOMBAT; }
        virtual void InitTriggers(std::list<TriggerNode*> &triggers);
        virtual string getName() { return "warsong"; }
    };

    class MountStrategy : public Strategy
    {
    public:
        MountStrategy(PlayerbotAI* ai) : Strategy(ai) {};
        virtual int GetType() { return STRATEGY_TYPE_NONCOMBAT; }
        virtual void InitTriggers(std::list<TriggerNode*> &triggers);
        virtual string getName() { return "mount"; }
    };
}
