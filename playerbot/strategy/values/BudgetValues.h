#pragma once

namespace ai
{
    class MoneyNeededValue : public Uint32CalculatedValue
    {
    public:
        MoneyNeededValue(PlayerbotAI* ai) : Uint32CalculatedValue(ai, "money needed",2) {}
        virtual uint32 Calculate();
    };

    class ShouldGetMoneyValue : public BoolCalculatedValue
    {
    public:
        ShouldGetMoneyValue(PlayerbotAI* ai) : BoolCalculatedValue(ai, "should get money") {}
        virtual bool Calculate() { return AI_VALUE(uint32, "money needed") > bot->GetMoney(); };
    };
}

