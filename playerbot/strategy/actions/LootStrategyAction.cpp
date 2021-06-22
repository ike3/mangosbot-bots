#include "botpch.h"
#include "../../playerbot.h"
#include "LootStrategyAction.h"
#include "../values/LootStrategyValue.h"
#include "LootAction.h"

using namespace ai;


bool LootStrategyAction::Execute(Event event)
{
    string strategy = event.getParam();

    Value<LootStrategy*>* lootStrategy = context->GetValue<LootStrategy*>("loot strategy");
    if (strategy == "?")
    {
        {
            ostringstream out;
            out << "Loot strategy: ";
            out << lootStrategy->Get()->GetName();
            ai->TellMaster(out);
        }

        TellLootList("always loot list");
        TellLootList("skip loot list");
    }
    else
    {
        ItemIds items = chat->parseItems(strategy);

        if (items.size() == 0)
        {
            lootStrategy->Set(LootStrategyValue::instance(strategy));
            ostringstream out;
            out << "Loot strategy set to " << lootStrategy->Get()->GetName();
            ai->TellMaster(out);
            return true;
        }

        bool clear = strategy.size() > 1 && strategy.substr(0, 1) == "!";
        bool remove = strategy.size() > 1 && strategy.substr(0, 1) == "-";
        bool query = strategy.size() > 1 && strategy.substr(0, 1) == "?";
        bool add = !clear && !remove && !query;
        bool changes = false;
        set<uint32>& alwaysLootItems = AI_VALUE(set<uint32>&, "always loot list");
        set<uint32>& skipLootItems = AI_VALUE(set<uint32>&, "skip loot list");
        for (ItemIds::iterator i = items.begin(); i != items.end(); i++)
        {
            uint32 itemid = *i;
            if (query)
            {
                ItemPrototype const *proto = sObjectMgr.GetItemPrototype(itemid);
                if (proto)
                {
                    ostringstream out;
                    out << (StoreLootAction::IsLootAllowed(itemid, ai) ? "|cFF000000Will loot " : "|c00FF0000Won't loot ") << ChatHelper::formatItem(proto);
                    ai->TellMaster(out.str());
                }
            }

            if (clear || add)
            {
                set<uint32>::iterator j = skipLootItems.find(itemid);
                if (j != skipLootItems.end()) skipLootItems.erase(j);
                changes = true;
            }

            if (clear || remove)
            {
                set<uint32>::iterator j = alwaysLootItems.find(itemid);
                if (j != alwaysLootItems.end()) alwaysLootItems.erase(j);
                changes = true;
            }

            if (remove)
            {
                skipLootItems.insert(itemid);
                changes = true;
            }

            if (add)
            {
                alwaysLootItems.insert(itemid);
                changes = true;
            }
        }

        if (changes)
        {
            TellLootList("always loot list");
            TellLootList("skip loot list");
        }
    }

    return true;
}

void LootStrategyAction::TellLootList(string name)
{
    set<uint32>& alwaysLootItems = AI_VALUE(set<uint32>&, name);
    ostringstream out;
    out << "My " << name << ":";

    for (set<uint32>::iterator i = alwaysLootItems.begin(); i != alwaysLootItems.end(); i++)
    {
        ItemPrototype const *proto = sItemStorage.LookupEntry<ItemPrototype>(*i);
        if (!proto)
            continue;

        out << " " << chat->formatItem(proto);
    }
    ai->TellMaster(out);
}