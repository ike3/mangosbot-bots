#include "botpch.h"
#include "../../playerbot.h"
#include "LootStrategyAction.h"
#include "../values/SubStrategyValue.h"
#include "../values/ItemUsageValue.h"
#include "LootAction.h"
#include "PlayerbotAIAware.h"

using namespace ai;

bool LootStrategyAction::Execute(Event& event)
{
    string strategy = event.getParam();
    Player* requester = event.getOwner() ? event.getOwner() : GetMaster();

    LootObjectStack* lootItems = AI_VALUE(LootObjectStack*, "available loot");
    set<uint32>& alwaysLootItems = AI_VALUE(set<uint32>&, "always loot list");
    set<uint32>& skipLootItems = AI_VALUE(set<uint32>&, "skip loot list");

    if (strategy == "?")
    {
        {
            ostringstream out;
            out << "Loot strategy: ";
            out << AI_VALUE(string, "loot strategy");
            ai->TellPlayer(requester, out);
        }

        TellLootList(requester, "always loot list");
        TellLootList(requester, "skip loot list");
    }
    else if (strategy == "clear")
    {
        alwaysLootItems.clear();
        skipLootItems.clear();
        ai->TellPlayer(requester, "My loot list is now empty");
        return true;
    }    
    else
    {
        set<string> itemQualifiers = chat->parseItemQualifiers(strategy);

        if (itemQualifiers.size() == 0)
        {
            SET_AI_VALUE(string, "loot strategy", strategy);

            string lootStrategy = AI_VALUE(string, "loot strategy");

            ostringstream out;
            out << "Loot strategy set to " << lootStrategy;
            ai->TellPlayer(requester, out);
            return true;
        }

        bool ignore = strategy.size() > 1 && strategy.substr(0, 1) == "!";
        bool remove = strategy.size() > 1 && strategy.substr(0, 1) == "-";
        bool query = strategy.size() > 1 && strategy.substr(0, 1) == "?";
        bool add = !ignore && !remove && !query;
        bool changes = false;
        for (auto& qualifier : itemQualifiers)
        {
            ItemQualifier itemQualifier(qualifier);
            auto itemid = itemQualifier.GetId();
            if (query)
            {
                if (itemQualifier.GetProto())
                {
                    ostringstream out;
                    out << (StoreLootAction::IsLootAllowed(itemQualifier, ai) ? "|cFF000000Will loot " : "|c00FF0000Won't loot ") << ChatHelper::formatItem(itemQualifier);
                    ai->TellPlayer(requester, out.str());
                }
            }
            
            if (remove || add)
            {
                set<uint32>::iterator j = skipLootItems.find(itemid);
                if (j != skipLootItems.end()) skipLootItems.erase(j);
                changes = true;
            }
            
            if (remove || ignore)
            {
                set<uint32>::iterator j = alwaysLootItems.find(itemid);
                if (j != alwaysLootItems.end()) alwaysLootItems.erase(j);
                changes = true;
            }
            
            if (ignore)
            {
                skipLootItems.insert(itemid);
                changes = true;
            }

            if (add)
            {
                alwaysLootItems.insert(itemid);
                changes = true;
            }

            if (changes)
            {
                TellLootList(requester, "always loot list");
                TellLootList(requester, "skip loot list");
                AI_VALUE(LootObjectStack*, "available loot")->Clear();
            }
        }
    }

    return true;
}

void LootStrategyAction::TellLootList(Player* requester, const string& name)
{
    set<uint32>& alwaysLootItems = AI_VALUE(set<uint32>&, name);
    ostringstream out;
    out << "My " << name << ":";

    for (set<uint32>::iterator i = alwaysLootItems.begin(); i != alwaysLootItems.end(); i++)
    {
        ItemPrototype const *proto = sItemStorage.LookupEntry<ItemPrototype>(*i);
        if (!proto)
        {
            continue;
        }

        out << " " << chat->formatItem(proto);
    }
    
    ai->TellPlayer(requester, out);
}