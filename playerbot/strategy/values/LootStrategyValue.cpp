#include "botpch.h"
#include "../../playerbot.h"
#include "LootStrategyValue.h"
#include "../values/ItemUsageValue.h"

using namespace ai;
using namespace std;

namespace ai
{
    class NormalLootStrategy : public LootStrategy
    {
    public:
        virtual bool CanLoot(ItemQualifier& itemQualifier, AiObjectContext *context)
        {
            ItemUsage usage = AI_VALUE2(ItemUsage, "item usage", itemQualifier.GetQualifier());
            return usage != ITEM_USAGE_NONE;
        }
        virtual string GetName() { return "normal"; }
    };

    class GrayLootStrategy : public NormalLootStrategy
    {
    public:
        virtual bool CanLoot(ItemQualifier& itemQualifier, AiObjectContext *context)
        {
            return NormalLootStrategy::CanLoot(itemQualifier, context) || itemQualifier.GetProto()->Quality == ITEM_QUALITY_POOR;
        }
        virtual string GetName() { return "gray"; }
    };

    class DisenchantLootStrategy : public NormalLootStrategy
    {
    public:
        virtual bool CanLoot(ItemQualifier& itemQualifier, AiObjectContext *context)
        {
            return NormalLootStrategy::CanLoot(itemQualifier, context) ||
                    (itemQualifier.GetProto()->Quality >= ITEM_QUALITY_UNCOMMON && itemQualifier.GetProto()->Bonding != BIND_WHEN_PICKED_UP &&
                    (itemQualifier.GetProto()->Class == ITEM_CLASS_ARMOR || itemQualifier.GetProto()->Class == ITEM_CLASS_WEAPON));
        }
        virtual string GetName() { return "disenchant"; }
    };

    class AllLootStrategy : public LootStrategy
    {
    public:
        virtual bool CanLoot(ItemQualifier& itemQualifier, AiObjectContext *context)
        {
            return true;
        }
        virtual string GetName() { return "all"; }
    };
}

LootStrategy *LootStrategyValue::normal = new NormalLootStrategy();
LootStrategy *LootStrategyValue::gray = new GrayLootStrategy();
LootStrategy *LootStrategyValue::disenchant = new DisenchantLootStrategy();
LootStrategy *LootStrategyValue::all = new AllLootStrategy();

LootStrategy* LootStrategyValue::instance(string strategy)
{
    if (strategy == "*" || strategy == "all")
        return all;

    if (strategy == "g" || strategy == "gray")
        return gray;

    if (strategy == "d" || strategy == "e" || strategy == "disenchant" || strategy == "enchant")
        return disenchant;

    return normal;
}

string LootStrategyValue::Save()
{
    return value ? value->GetName() : "?";
}

bool LootStrategyValue::Load(string text)
{
    value = LootStrategyValue::instance(text);
    return true;
}
