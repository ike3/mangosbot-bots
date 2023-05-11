#pragma once
#include "StatsValues.h"

namespace ai
{
    class AttackersCountValue : public Uint8CalculatedValue, public Qualified
    {
    public:
        AttackersCountValue(PlayerbotAI* ai, string name = "attackers count") : Uint8CalculatedValue(ai, name, 4), Qualified() {}
        virtual uint8 Calculate();
    };

    class PossibleAttackTargetsCountValue : public Uint8CalculatedValue, public Qualified
    {
    public:
        PossibleAttackTargetsCountValue(PlayerbotAI* ai, string name = "possible attack targets count") : Uint8CalculatedValue(ai, name, 4), Qualified() {}
        virtual uint8 Calculate();
    };

    class HasAttackersValue : public BoolCalculatedValue, public Qualified
    {
    public:
        HasAttackersValue(PlayerbotAI* ai, string name = "has attackers") : BoolCalculatedValue(ai, name, 4), Qualified() {}
        virtual bool Calculate();
    };

    class HasPossibleAttackTargetsValue : public BoolCalculatedValue, public Qualified
    {
    public:
        HasPossibleAttackTargetsValue(PlayerbotAI* ai, string name = "has possible attack targets") : BoolCalculatedValue(ai, name, 4), Qualified() {}
        virtual bool Calculate();
    };

    class MyAttackerCountValue : public Uint8CalculatedValue, public Qualified
    {
    public:
        MyAttackerCountValue(PlayerbotAI* ai, string name = "my attackers count") : Uint8CalculatedValue(ai, name, 4), Qualified() {}

        Unit* GetTarget()
        {
            AiObjectContext* ctx = AiObject::context;
            return ctx->GetValue<Unit*>(qualifier)->Get();
        }
        virtual uint8 Calculate();
    };

    class HasAggroValue : public BoolCalculatedValue, public Qualified
    {
    public:
        HasAggroValue(PlayerbotAI* ai, string name = "has agro") : BoolCalculatedValue(ai, name, 4), Qualified() {}

        Unit* GetTarget()
        {
            AiObjectContext* ctx = AiObject::context;
            return ctx->GetValue<Unit*>(qualifier)->Get();
        }
        virtual bool Calculate();
    };

    class BalancePercentValue : public Uint8CalculatedValue, public Qualified
    {
    public:
        BalancePercentValue(PlayerbotAI* ai, string name = "balance percentage") : Uint8CalculatedValue(ai, name, 4), Qualified() {}

        Unit* GetTarget()
        {
            AiObjectContext* ctx = AiObject::context;
            return ctx->GetValue<Unit*>(qualifier)->Get();
        }
        virtual uint8 Calculate();
    };

}
