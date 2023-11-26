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

    if (strategy == "?")
    {
        {
            ostringstream out;
            out << "Loot strategy: ";
            out << AI_VALUE(string, "loot strategy");
            ai->TellPlayer(requester, out);
        }

        {
            ostringstream out;
            out << "Always loot items: ";
            for (set<uint32>::iterator i = alwaysLootItems.begin(); i != alwaysLootItems.end(); i++)
            {
                ItemPrototype const *proto = sItemStorage.LookupEntry<ItemPrototype>(*i);
                if (!proto)
                {
                    continue;
                }

                out << chat->formatItem(proto);
            }

            ai->TellPlayer(requester, out);
        }
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
                    out << (StoreLootAction::IsLootAllowed(itemQualifier, ai) ? "|cFF000000Will loot " : "|c00FF0000Won't loot ") << ChatHelper::formatItem(itemQualifier);
                    ai->TellPlayer(requester, out.str());
                }
            }
            else if (remove)
            {
                set<uint32>::iterator j = alwaysLootItems.find(itemQualifier.GetId());
                if (j != alwaysLootItems.end())
                {
                    alwaysLootItems.erase(j);
                }

                ai->TellPlayer(requester, "Item(s) removed from always loot list");
            }
            else
            {
                alwaysLootItems.insert(itemQualifier.GetId());
                ai->TellPlayer(requester, "Item(s) added to always loot list");
            }
        }
    }

    return true;
}

