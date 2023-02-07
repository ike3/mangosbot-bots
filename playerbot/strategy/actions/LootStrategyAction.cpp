#include "botpch.h"
#include "../../playerbot.h"
#include "LootStrategyAction.h"
#include "../values/LootStrategyValue.h"
#include "../values/ItemUsageValue.h"
#include "LootAction.h"
#include "PlayerbotAIAware.h"

using namespace ai;

bool LootStrategyAction::Execute(Event& event)
{
    string strategy = event.getParam();

    LootObjectStack* lootItems = AI_VALUE(LootObjectStack*, "available loot");
    set<uint32>& alwaysLootItems = AI_VALUE(set<uint32>&, "always loot list");
    Value<LootStrategy*>* lootStrategy = context->GetValue<LootStrategy*>("loot strategy");

    if (strategy == "?")
    {
        {
            ostringstream out;
            out << "Loot strategy: ";
            out << lootStrategy->Get()->GetName();
            ai->TellMaster(out);
        }

        {
            ostringstream out;
            out << "Always loot items: ";

            for (set<uint32>::iterator i = alwaysLootItems.begin(); i != alwaysLootItems.end(); i++)
            {
                ItemPrototype const *proto = sItemStorage.LookupEntry<ItemPrototype>(*i);
                if (!proto)
                    continue;

                out << chat->formatItem(proto);
            }
            ai->TellMaster(out);
        }
    }
    else
    {
        set<string> itemQualifiers = chat->parseItemQualifiers(strategy);

        if (itemQualifiers.size() == 0)
        {
            lootStrategy->Set(LootStrategyValue::instance(strategy));
            ostringstream out;
            out << "Loot strategy set to " << lootStrategy->Get()->GetName();
            ai->TellMaster(out);
            return true;
        }

        bool remove = strategy.size() > 1 && strategy.substr(0, 1) == "-";
        bool query = strategy.size() > 1 && strategy.substr(0, 1) == "?";
        for (auto& qualifier : itemQualifiers)
        {
            ItemQualifier itemQualifier(qualifier);
            if (query)
            {
                if (itemQualifier.GetProto())
                {
                    ostringstream out;
                    out << (StoreLootAction::IsLootAllowed(itemQualifier, ai) ? "|cFF000000Will loot " : "|c00FF0000Won't loot ") << ChatHelper::formatItem(itemQualifier.GetProto());
                    ai->TellMaster(out.str());
                }
            }
            else if (remove)
            {
                set<uint32>::iterator j = alwaysLootItems.find(itemQualifier.GetId());
                if (j != alwaysLootItems.end())
                    alwaysLootItems.erase(j);

                ai->TellMaster("Item(s) removed from always loot list");
            }
            else
            {
                alwaysLootItems.insert(itemQualifier.GetId());
                ai->TellMaster("Item(s) added to always loot list");
            }
        }
    }

    return true;
}

