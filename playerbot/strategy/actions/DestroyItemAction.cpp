#include "botpch.h"
#include "../../playerbot.h"
#include "DestroyItemAction.h"

#include "../values/ItemCountValue.h"

using namespace ai;

bool DestroyItemAction::Execute(Event event)
{
    string text = event.getParam();
    ItemIds ids = chat->parseItems(text);

    for (ItemIds::iterator i =ids.begin(); i != ids.end(); i++)
    {
        FindItemByIdVisitor visitor(*i);
        DestroyItem(&visitor);
    }

    return true;
}

void DestroyItemAction::DestroyItem(FindItemVisitor* visitor)
{
    IterateItems(visitor);
    list<Item*> items = visitor->GetResult();
	for (list<Item*>::iterator i = items.begin(); i != items.end(); ++i)
    {
		Item* item = *i;
        bot->DestroyItem(item->GetBagSlot(),item->GetSlot(), true);
        ostringstream out; out << chat->formatItem(item->GetProto()) << " destroyed";
        ai->TellMaster(out);
    }
}

bool SmartDestroyItemAction::Execute(Event event)
{
    uint8 bagSpace = AI_VALUE(uint8, "bag space");

    if (bagSpace < 90)
        return false;

    vector<uint32> bestToDestroy = { ITEM_USAGE_NONE };

    if (!AI_VALUE(bool, "can sell") && AI_VALUE(bool, "should get money"))
        bestToDestroy.push_back(ITEM_USAGE_QUEST);
    else
    {
        bestToDestroy.push_back(ITEM_USAGE_VENDOR);
        bestToDestroy.push_back(ITEM_USAGE_AH);
    }

    for (auto& usage : bestToDestroy)
    {

        list<Item*> items = AI_VALUE2(list<Item*>, "inventory items", "quest");

        items.reverse();

        for (auto& item : items)
        {
            FindItemByIdVisitor visitor(item->GetProto()->ItemId);
            DestroyItem(&visitor);

            bagSpace = AI_VALUE(uint8, "bag space");

            if(bagSpace < 90)
                return true;
        }
    }

    return false;
}