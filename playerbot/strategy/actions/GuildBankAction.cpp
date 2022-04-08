#include "botpch.h"
#include "../../playerbot.h"
#include "GuildBankAction.h"

#include "../values/ItemCountValue.h"
#include "Guild.h"
#include "GuildMgr.h"

using namespace std;
using namespace ai;

bool GuildBankAction::Execute(Event event)
{
#ifndef MANGOSBOT_ZERO
    string text = event.getParam();
    if (text.empty())
        return false;

    if (!bot->GetGuildId() || (GetMaster() && GetMaster()->GetGuildId() != bot->GetGuildId()))
    {
        ai->TellMaster("I'm not in your guild!");
            return false;
    }

    list<ObjectGuid> gos = *ai->GetAiObjectContext()->GetValue<list<ObjectGuid> >("nearest game objects");
    for (list<ObjectGuid>::iterator i = gos.begin(); i != gos.end(); ++i)
    {
        GameObject* go = ai->GetGameObject(*i);
        if (!go || !bot->GetGameObjectIfCanInteractWith(go->GetObjectGuid(), GAMEOBJECT_TYPE_GUILD_BANK))
            continue;

        return Execute(text, go);
    }

    ai->TellMaster(BOT_TEXT("error_gbank_found"));
    return false;
#else
    return false;
#endif
}

bool GuildBankAction::Execute(string text, GameObject* bank)
{
    bool result = true;

    list<Item*> found = parseItems(text);
    if (found.empty())
        return false;

    for (list<Item*>::iterator i = found.begin(); i != found.end(); i++)
    {
        Item* item = *i;
        if (item)
            result &= MoveFromCharToBank(item, bank);
    }

    return result;
}

bool GuildBankAction::MoveFromCharToBank(Item* item, GameObject* bank)
{
#ifndef MANGOSBOT_ZERO
    uint32 playerSlot = item->GetSlot();
    uint32 playerBag = item->GetBagSlot();
    ostringstream out;

    Guild* guild = sGuildMgr.GetGuildById(bot->GetGuildId());
    //guild->SwapItems(bot, 0, playerSlot, 0, INVENTORY_SLOT_BAG_0, 0);

    // check source pos rights (item moved to bank)
    if (!guild->IsMemberHaveRights(bot->GetGUIDLow(), 0, GUILD_BANK_RIGHT_DEPOSIT_ITEM))
        out << BOT_TEXT("error_cant_put") << chat->formatItem(item->GetProto()) << BOT_TEXT("error_gbank_rights");
    else
    {
        out << chat->formatItem(item->GetProto()) << BOT_TEXT("gbank_put");
        guild->MoveFromCharToBank(bot, playerBag, playerSlot, 0, 255, 0);
    }

    ai->TellMaster(out);
    return true;
#else
    return false;
#endif
}
