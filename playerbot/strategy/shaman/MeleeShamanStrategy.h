#pragma once

#include "GenericShamanStrategy.h"

namespace ai
{
    class MeleeShamanStrategy : public GenericShamanStrategy
    {
    public:
        MeleeShamanStrategy(PlayerbotAI* ai);

    public:
        virtual void InitTriggers(std::list<TriggerNode*> &triggers);
        virtual NextAction** getDefaultActions();
        virtual string getName() { return "melee"; }
        virtual int GetType() { return STRATEGY_TYPE_COMBAT | STRATEGY_TYPE_DPS | STRATEGY_TYPE_MELEE; }
    };

    class MeleeAoeShamanStrategy : public CombatStrategy
    {
    public:
        MeleeAoeShamanStrategy(PlayerbotAI* ai) : CombatStrategy(ai) {}

    public:
        virtual void InitTriggers(std::list<TriggerNode*> &triggers);
        virtual string getName() { return "melee aoe"; }
    };

    class EarthShamanStrategy : public CombatStrategy
    {
    public:
        EarthShamanStrategy(PlayerbotAI* ai) : CombatStrategy(ai) {}

    public:
        virtual void InitTriggers(std::list<TriggerNode*> &triggers);
        virtual string getName() { return "earth"; }
    };

    class FireShamanStrategy : public CombatStrategy
    {
    public:
        FireShamanStrategy(PlayerbotAI* ai) : CombatStrategy(ai) {}

    public:
        virtual void InitTriggers(std::list<TriggerNode*> &triggers);
        virtual string getName() { return "fire"; }
    };

    class FrostShamanStrategy : public CombatStrategy
    {
    public:
        FrostShamanStrategy(PlayerbotAI* ai) : CombatStrategy(ai) {}

    public:
        virtual void InitTriggers(std::list<TriggerNode*> &triggers);
        virtual string getName() { return "frost"; }
    };

    class AirShamanStrategy : public CombatStrategy
    {
    public:
        AirShamanStrategy(PlayerbotAI* ai) : CombatStrategy(ai) {}

    public:
        virtual void InitTriggers(std::list<TriggerNode*> &triggers);
        virtual string getName() { return "air"; }
    };

}
