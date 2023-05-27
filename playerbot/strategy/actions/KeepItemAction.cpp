#include "botpch.h"
#include "../../playerbot.h"
#include "KeepItemAction.h"

#include "../values/ItemCountValue.h"
#include "../values/ItemUsageValue.h"

using namespace ai;

bool KeepItemAction::Execute(Event& event)
{
    string text = event.getParam();

    string type = text.substr(0, text.find(" "));

    if (text.empty()) //No param = help feedback
    {
        ostringstream out;
        out << "Please specify the items that should be kept. See " << ChatHelper::formatValue("help", "action:keep", "keep help") << " for more information.";
        ai->TellPlayer(GetMaster(), out.str());
        return false;
    }
    else if (string("none,keep,equip,greed,need,?,").find(type + ",") == string::npos) //Non type = using keep.
        type = "keep";
    else if(text.find(" ") != string::npos) //Remove type from param.
        text = text.substr(text.find(" ") + 1);

    ItemIds ids;

    if (text == "all")
    {
        FindNamedItemVisitor visitor(bot, "");
        ai->InventoryIterateItems(&visitor, ITERATE_ALL_ITEMS);

        list<Item*> items = visitor.GetResult();

        for (auto& item : items)
            ids.insert(item->GetProto()->ItemId);
    }
    else if (text == "equip")
    {
        FindNamedItemVisitor visitor(bot, "");
        ai->InventoryIterateItems(&visitor, ITERATE_ITEMS_IN_EQUIP);

        list<Item*> items = visitor.GetResult();

        for (auto& item : items)
            ids.insert(item->GetProto()->ItemId);
    }

    if (ids.empty())
    {
        list<Item*> items = ai->InventoryParseItems(text);

        for (auto& item : items)
                ids.insert(item->GetProto()->ItemId);
    }

    if (ids.empty())
        ids = chat->parseItems(text);

    if (ids.empty())
    {
        ostringstream out;

        if (type == "keep")
            out << "Please specify the items that should be kept. See " << ChatHelper::formatValue("help", "action:keep", "keep help") << " for more information.";
        else
            out << "No items found.";

        ai->TellPlayer(GetMaster(), out.str());
        return true;
    }

    if (type == "?")
    {
        for (auto& id : ids)
        {
            const ItemPrototype* proto = sObjectMgr.GetItemPrototype(id);

            if (!proto)
                continue;

            ostringstream out;
            out << chat->formatItem(proto);
            out << ": " << keepName[AI_VALUE2_EXISTS(ForceItemUsage, "force item usage", id, ForceItemUsage::FORCE_USAGE_NEED)] << " the item.";
            ai->TellPlayer(GetMaster(), out.str());

        }
        return true;
    }

    uint32 changed = 0;

    ForceItemUsage usage;
    if (type == "none")
        usage = ForceItemUsage::FORCE_USAGE_NONE;
    else if (type == "keep")
        usage = ForceItemUsage::FORCE_USAGE_KEEP;
    else if (type == "equip")
        usage = ForceItemUsage::FORCE_USAGE_EQUIP;
    else if (type == "greed")
        usage = ForceItemUsage::FORCE_USAGE_GREED;
    else if (type == "need")
        usage = ForceItemUsage::FORCE_USAGE_NEED;

    for (auto& id : ids)
    {
        if (AI_VALUE2_EXISTS(ForceItemUsage, "force item usage", id, ForceItemUsage::FORCE_USAGE_NONE) != usage)
        {
            changed++;
            SET_AI_VALUE2(ForceItemUsage, "force item usage", id, usage);
        }
    }

    ostringstream out;
    out << changed;
    out << " items changed to: " << keepName[usage] << " the item.";
    ai->TellPlayer(GetMaster(), out.str());

    sPlayerbotDbStore.Save(ai);

    return true;
}