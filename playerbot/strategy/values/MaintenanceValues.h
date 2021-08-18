#pragma once
#include "../Value.h"
#include "../values/ItemUsageValue.h"

namespace ai
{
    class ShouldHomeBindValue : public BoolCalculatedValue
    {
    public:
        ShouldHomeBindValue(PlayerbotAI* ai) : BoolCalculatedValue(ai, "should home bind", 2) {}
        virtual bool Calculate() { return AI_VALUE2(float, "distance", "home bind") > 1000.0f; };
    };


    class ShouldRepairValue : public BoolCalculatedValue
	{
	public:
        ShouldRepairValue(PlayerbotAI* ai) : BoolCalculatedValue(ai,"should repair",2) {}
        virtual bool Calculate() { return AI_VALUE(uint8, "durability") < 80; };
    };

    class CanRepairValue : public BoolCalculatedValue
    {
    public:
        CanRepairValue(PlayerbotAI* ai) : BoolCalculatedValue(ai, "can repair",2) {}
        virtual bool Calculate() { return AI_VALUE(uint8, "durability") < 100 && AI_VALUE(uint32, "repair cost") < bot->GetMoney(); };
    };

    class ShouldSellValue : public BoolCalculatedValue
    {
    public:
        ShouldSellValue(PlayerbotAI* ai) : BoolCalculatedValue(ai, "should sell",2) {}
        virtual bool Calculate() { return AI_VALUE(uint8, "bag space") > 80; };
    };

    class CanSellValue : public BoolCalculatedValue
    {
    public:
        CanSellValue(PlayerbotAI* ai) : BoolCalculatedValue(ai, "can sell",2) {}
        virtual bool Calculate() { return (AI_VALUE2(uint32, "item count", "usage " + ITEM_USAGE_VENDOR) + AI_VALUE2(uint32, "item count", "usage " + ITEM_USAGE_AH)) > 1; };
    };

    class CanFightEqualValue: public BoolCalculatedValue
    {
    public:
        CanFightEqualValue(PlayerbotAI* ai) : BoolCalculatedValue(ai, "can fight equal",2) {}
        virtual bool Calculate() { return AI_VALUE(uint8, "durability") > 20; };
    };

    class CanFightBossValue : public BoolCalculatedValue
    {
    public:
        CanFightBossValue(PlayerbotAI* ai) : BoolCalculatedValue(ai, "can fight boss") {}
        virtual bool Calculate() { return AI_VALUE(bool, "can fight equal") && bot->GetGroup() && bot->GetGroup()->GetMembersCount() > 2; };
    };  
}
