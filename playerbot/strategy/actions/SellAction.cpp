#include "botpch.h"
#include "../../playerbot.h"
#include "SellAction.h"
#include "../ItemVisitors.h"
#include "../values/ItemUsageValue.h"

using namespace ai;

class SellItemsVisitor : public IterateItemsVisitor
{
public:
    SellItemsVisitor(SellAction* action) : IterateItemsVisitor()
    {
        this->action = action;
    }

    virtual bool Visit(Item* item)
    {
        action->Sell(item);
        return true;
    }

private:
    SellAction* action;
};

class SellGrayItemsVisitor : public SellItemsVisitor
{
public:
    SellGrayItemsVisitor(SellAction* action) : SellItemsVisitor(action) {}

    virtual bool Visit(Item* item)
    {
        if (item->GetProto()->Quality != ITEM_QUALITY_POOR)
            return true;

        return SellItemsVisitor::Visit(item);
    }
};

class SellVendorItemsVisitor : public SellItemsVisitor
{
public:
    SellVendorItemsVisitor(SellAction* action, AiObjectContext* con) : SellItemsVisitor(action) { context = con; }

    AiObjectContext* context;

    virtual bool Visit(Item* item)
    {
        RESET_AI_VALUE2(ItemUsage, "item usage", ItemQualifier(item).GetQualifier()); //Recheck if we still want to sell this.

        ItemUsage usage = AI_VALUE2(ItemUsage, "item usage", ItemQualifier(item).GetQualifier());

        bool shouldSell = false;

        if (usage == ITEM_USAGE_VENDOR)
            shouldSell = true;
        else if (usage == ITEM_USAGE_AH && AI_VALUE(uint8, "bag space") > 80 && !urand(0, 10))
            shouldSell = true;

        if (!shouldSell)
            return true;

        return SellItemsVisitor::Visit(item);
    }
};

bool SellAction::Execute(Event& event)
{
    string text = event.getParam();

    if (text == "gray" || text == "*")
    {
        SellGrayItemsVisitor visitor(this);
        ai->InventoryIterateItems(&visitor);
        return true;
    }

    if (text == "vendor")
    {
        SellVendorItemsVisitor visitor(this, context);
        ai->InventoryIterateItems(&visitor);
        return true;
    }

    list<Item*> items = ai->InventoryParseItems(text, ITERATE_ITEMS_IN_BAGS);
    for (list<Item*>::iterator i = items.begin(); i != items.end(); ++i)
    {
        Sell(*i);
    }

    return true;
}

void SellAction::Sell(FindItemVisitor* visitor)
{
    ai->InventoryIterateItems(visitor);
    list<Item*> items = visitor->GetResult();
    for (list<Item*>::iterator i = items.begin(); i != items.end(); ++i)
        Sell(*i);
}

void SellAction::Sell(Item* item)
{
    ostringstream out;
    list<ObjectGuid> vendors = ai->GetAiObjectContext()->GetValue<list<ObjectGuid> >("nearest npcs")->Get();

    for (list<ObjectGuid>::iterator i = vendors.begin(); i != vendors.end(); ++i)
    {
        ObjectGuid vendorguid = *i;
        Creature *pCreature = bot->GetNPCIfCanInteractWith(vendorguid,UNIT_NPC_FLAG_VENDOR);
        if (!pCreature)
            continue;     

        ObjectGuid itemguid = item->GetObjectGuid();
        uint32 count = item->GetCount();

        uint32 botMoney = bot->GetMoney();

        sPlayerbotAIConfig.logEvent(ai, "SellAction", item->GetProto()->Name1, to_string(item->GetProto()->ItemId));

        WorldPacket p;
        p << vendorguid << itemguid << count;
        bot->GetSession()->HandleSellItemOpcode(p);

        if (ai->HasCheat(BotCheatMask::gold))
        {
            bot->SetMoney(botMoney);
        }

        out << "Selling " << chat->formatItem(item);
        if (sPlayerbotAIConfig.globalSoundEffects)
            bot->PlayDistanceSound(120);
        ai->TellMaster(out, PlayerbotSecurityLevel::PLAYERBOT_SECURITY_ALLOW_ALL, false);
        break;
    }
}