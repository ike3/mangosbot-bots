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

    list<ObjectGuid> gos = *ai->GetAiObjectContext()->GetValue<list<ObjectGuid> >("nearest game objects");
    for (list<ObjectGuid>::iterator i = gos.begin(); i != gos.end(); ++i)
    {
        GameObject* go = ai->GetGameObject(*i);
        if (!go || !bot->GetGameObjectIfCanInteractWith(go->GetObjectGuid(), GAMEOBJECT_TYPE_GUILD_BANK))
            continue;

        return Execute(text, go);
    }

    ai->TellMaster("Cannot find the guild bank nearby");
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

    Guild* guild = sGuildMgr.GetGuildById(bot->GetGuildId());
    guild->SwapItems(bot, 0, playerSlot, 0, INVENTORY_SLOT_BAG_0, 0);

    ostringstream out; out << chat->formatItem(item->GetProto()) << " put to guild bank";
    ai->TellMaster(out);
    return true;
#else
    return false;
#endif
}
