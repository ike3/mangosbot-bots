#pragma once

#include "GenericWarriorStrategy.h"

namespace ai
{
    class ArmsWarriorStrategy : public GenericWarriorStrategy
    {
    public:
        ArmsWarriorStrategy(PlayerbotAI* ai);

    public:
        virtual void InitTriggers(std::list<TriggerNode*> &triggers);
        virtual string getName() { return "arms"; }
        virtual NextAction** getDefaultActions();
        virtual int GetType() { return STRATEGY_TYPE_COMBAT | STRATEGY_TYPE_DPS | STRATEGY_TYPE_MELEE; }
    };

    class ArmsWarrirorAoeStrategy : public CombatStrategy
    {
    public:
        ArmsWarrirorAoeStrategy(PlayerbotAI* ai) : CombatStrategy(ai) {}

    public:
        virtual void InitTriggers(std::list<TriggerNode*> &triggers);
        virtual string getName() { return "arms aoe"; }
    };
}
