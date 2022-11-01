#pragma once
#include "StatsValues.h"

namespace ai
{

    class AttackerCountValue : public Uint8CalculatedValue, public Qualified
    {
    public:
        AttackerCountValue(PlayerbotAI* ai, string name = "attackers count") : Uint8CalculatedValue(ai, name, 3) {}

        Unit* GetTarget()
        {
            AiObjectContext* ctx = AiObject::context;
            return ctx->GetValue<Unit*>(qualifier)->Get();
        }
        virtual uint8 Calculate();
    };

    class MyAttackerCountValue : public Uint8CalculatedValue, public Qualified
    {
    public:
        MyAttackerCountValue(PlayerbotAI* ai, string name = "my attackers count") : Uint8CalculatedValue(ai, name, 3) {}

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
        HasAggroValue(PlayerbotAI* ai, string name = "has agro") : BoolCalculatedValue(ai, name, 3) {}

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
        BalancePercentValue(PlayerbotAI* ai, string name = "balance percentage") : Uint8CalculatedValue(ai, name, 3) {}

        Unit* GetTarget()
        {
            AiObjectContext* ctx = AiObject::context;
            return ctx->GetValue<Unit*>(qualifier)->Get();
        }
        virtual uint8 Calculate();
    };

}
