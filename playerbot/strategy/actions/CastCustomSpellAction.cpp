#include "botpch.h"
#include "../../playerbot.h"
#include "CastCustomSpellAction.h"

#include "../../PlayerbotAIConfig.h"
#include "../../ServerFacade.h"

using namespace ai;

int FindLastSeparator(string text, string sep)
{
    int pos = text.rfind(sep);
    if (pos == string::npos) return pos;

    int lastLinkBegin = text.rfind("|H");
    int lastLinkEnd = text.find("|h|r", lastLinkBegin + 1);
    if (pos >= lastLinkBegin && pos <= lastLinkEnd)
        pos = text.find_last_of(sep, lastLinkBegin);

    return pos;
}

static inline void ltrim(std::string& s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
        return !std::isspace(ch);
        }));
}

bool CastCustomSpellAction::Execute(Event& event)
{
    // only allow proper vehicle seats
    if (ai->IsInVehicle() && !ai->IsInVehicle(false, false, true))
        return false;

    Unit* target = NULL;
    string text = event.getParam();
    Player* master = GetMaster();

    list<ObjectGuid> gos = chat->parseGameobjects(text);

    if (!gos.empty())
    {
        for (auto go : gos)
        {
            if (!target)
                target = ai->GetUnit(go);

            chat->eraseAllSubStr(text, chat->formatWorldobject(ai->GetUnit(go)));
        }

        ltrim(text);
    }
    
    if (!target)
        if (master && master->GetSelectionGuid())
            target = ai->GetUnit(master->GetSelectionGuid());

    if (!target)
        target = bot;

    if (!master) //Use self as master for permissions.
        master = bot;

    Item* itemTarget = NULL;

    int pos = FindLastSeparator(text, " ");
    int castCount = 1;
    if (pos != string::npos)
    {
        string param = text.substr(pos + 1);
        list<Item*> items = InventoryAction::parseItems(param, ITERATE_ITEMS_IN_BAGS);
        if (!items.empty()) itemTarget = *items.begin();
        else
        {
            castCount = atoi(param.c_str());
            if (castCount > 0)
                text = text.substr(0, pos);
        }
    }

    uint32 spell = AI_VALUE2(uint32, "spell id", text);

    ostringstream msg;
    if (!spell)
    {
        msg << "Unknown spell " << text;
        ai->TellError(msg.str());
        return false;
    }

    SpellEntry const* pSpellInfo = sServerFacade.LookupSpellInfo(spell);
    if (!pSpellInfo)
    {
        msg << "Unknown spell " << text;
        ai->TellError(msg.str());
        return false;
    }

    if (target != bot && !sServerFacade.IsInFront(bot, target, sPlayerbotAIConfig.sightDistance, CAST_ANGLE_IN_FRONT))
    {
        sServerFacade.SetFacingTo(bot, target);
        SetDuration(sPlayerbotAIConfig.globalCoolDown);
        msg << "cast " << text;
        ai->HandleCommand(CHAT_MSG_WHISPER, msg.str(), *master);
        return true;
    }

    ostringstream spellName;
    spellName << ChatHelper::formatSpell(pSpellInfo) << " on ";
    if (bot->GetTrader()) spellName << "trade item";
    else if (itemTarget) spellName << chat->formatItem(itemTarget->GetProto());
    else if (target == bot) spellName << "self";
    else spellName << target->GetName();

    if (!bot->GetTrader() && !ai->CanCastSpell(spell, target, 0, true, itemTarget))
    {
        msg << "Cannot cast " << spellName.str();
        ai->TellError(msg.str());
        return false;
    }

    MotionMaster& mm = *bot->GetMotionMaster();

    bool result = spell ? ai->CastSpell(spell, target, itemTarget) : ai->CastSpell(text, target, itemTarget);
    if (result)
    {
        msg << "Casting " << spellName.str();

        if (castCount > 1)
        {
            ostringstream cmd;
            cmd << castString(target) << " " << text << " " << (castCount - 1);
            ai->HandleCommand(CHAT_MSG_WHISPER, cmd.str(), *master);
            msg << "|cffffff00(x" << (castCount - 1) << " left)|r";
        }
        ai->TellMasterNoFacing(msg.str(), PlayerbotSecurityLevel::PLAYERBOT_SECURITY_ALLOW_ALL, false);
    }
    else
    {
        msg << "Cast " << spellName.str() << " is failed";
        ai->TellError(msg.str());
    }

    return result;
}


bool CastRandomSpellAction::Execute(Event& event)
{
    list<pair<uint32, string>> spellMap = GetSpellList();
    Player* master = GetMaster();
    
    Unit* target = nullptr;
    GameObject* got = nullptr;

    string name = event.getParam();
    if (name.empty())
        name = getName();

    list<ObjectGuid> wos = chat->parseGameobjects(name);

    for (auto wo : wos)
    {
        target = ai->GetUnit(wo);
        got = ai->GetGameObject(wo);

        if (got || target)
            break;
    }    

    if (!got && !target && bot->GetSelectionGuid())
    {
        target = ai->GetUnit(bot->GetSelectionGuid());
        got = ai->GetGameObject(bot->GetSelectionGuid());
    }

    if (!got && !target)
        if (master && master->GetSelectionGuid())
            target = ai->GetUnit(master->GetSelectionGuid());

    if (!got && !target)
        target = bot;

    vector<pair<uint32, pair<uint32, WorldObject*>>> spellList;

    for (auto & spell : spellMap)
    {
        uint32 spellId = spell.first;

        const SpellEntry* pSpellInfo = sServerFacade.LookupSpellInfo(spellId);
        if (!pSpellInfo)
            continue;

        if (!AcceptSpell(pSpellInfo))
            continue;

        if (bot->HasSpell(spellId))
        {
            uint32 spellPriority = GetSpellPriority(pSpellInfo);

            if (target && ai->CanCastSpell(spellId, target, true))
                spellList.push_back(make_pair(spellId,make_pair(spellPriority, target)));
            if (got && ai->CanCastSpell(spellId, got->GetPositionX(), got->GetPositionY(), got->GetPositionZ(), true))
                spellList.push_back(make_pair(spellId, make_pair(spellPriority, got)));
            if (ai->CanCastSpell(spellId, bot, true))
                spellList.push_back(make_pair(spellId, make_pair(spellPriority, bot)));
        }
    }

    if (spellList.empty())
        return false;

    bool isCast = false;

    std::sort(spellList.begin(), spellList.end(), [](pair<uint32, pair<uint32, WorldObject*>> i, pair<uint32, pair<uint32, WorldObject*>> j) {return i.first > j.first; });

    uint32 rndBound = spellList.size() / 4;

    rndBound = std::min(rndBound, (uint32)10);
    rndBound = std::max(rndBound, (uint32)0);

    for (uint32 i = 0; i < 5; i++)
    {
        uint32 rnd = urand(0, rndBound);

        auto spell = spellList[rnd];

        uint32 spellId = spell.first;
        WorldObject* wo = spell.second.second;

        bool isCast = castSpell(spellId, wo);

        if (isCast)
        {
            if (MultiCast && ((wo && sServerFacade.IsInFront(bot, wo, sPlayerbotAIConfig.sightDistance, CAST_ANGLE_IN_FRONT))))
            {               
                ostringstream cmd;

                cmd << "castnc " << chat->formatWorldobject(wo) + " " << spellId << " " << 19;

                ai->HandleCommand(CHAT_MSG_WHISPER, cmd.str(), *bot);
            }
            return true;
        }
    }

    return false;
}

bool CastRandomSpellAction::castSpell(uint32 spellId, WorldObject* wo)
{

    if (wo->GetObjectGuid().IsUnit())
        return ai->CastSpell(spellId, (Unit*)(wo));
    else
        return ai->CastSpell(spellId, wo->GetPositionX(), wo->GetPositionY(), wo->GetPositionZ());
}

bool DisEnchantRandomItemAction::Execute(Event& event)
{
    list<uint32> items = AI_VALUE2(list<uint32>, "inventory item ids", "usage " + to_string(ITEM_USAGE_DISENCHANT));

    items.reverse();

    if (bot->IsMoving())
    {
        ai->StopMoving();
    }
    if (bot->IsMounted())
    {
        return false;
    }

    for (auto& item: items)
    {
        // don't touch rare+ items if with real player/guild
        if ((ai->HasRealPlayerMaster() || ai->IsInRealGuild()) && ObjectMgr::GetItemPrototype(item)->Quality > ITEM_QUALITY_UNCOMMON)
            return false;

        Event disenchantEvent = Event("disenchant random item", "13262 " + chat->formatQItem(item));
        const bool used = CastCustomSpellAction::Execute(disenchantEvent);
        if (used)
        {
            SetDuration(3000U); // 3s
        }

        return used;
    }

    return false;
};


