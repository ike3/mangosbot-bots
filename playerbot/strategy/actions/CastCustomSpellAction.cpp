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

bool CastCustomSpellAction::Execute(Event event)
{
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

    SpellEntry const *pSpellInfo = sServerFacade.LookupSpellInfo(spell);
    if (!pSpellInfo)
    {
        msg << "Unknown spell " << text;
        ai->TellError(msg.str());
        return false;
    }

    if (target != bot && !sServerFacade.IsInFront(bot, target, sPlayerbotAIConfig.sightDistance, CAST_ANGLE_IN_FRONT))
    {
        sServerFacade.SetFacingTo(bot, target);
        ai->SetNextCheckDelay(sPlayerbotAIConfig.globalCoolDown);
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

    if (!bot->GetTrader() && !ai->CanCastSpell(spell, target, true, itemTarget))
    {
        msg << "Cannot cast " << spellName.str();
        ai->TellError(msg.str());
        return false;
    }

    MotionMaster &mm = *bot->GetMotionMaster();

    bool result = spell ? ai->CastSpell(spell, target, itemTarget) : ai->CastSpell(text, target, itemTarget);
    if (result)
    {
        msg << "Casting " << spellName.str();

        if (castCount > 1)
        {
            ostringstream cmd;
            cmd << "cast " << text << " " << (castCount - 1);
            ai->HandleCommand(CHAT_MSG_WHISPER, cmd.str(), *master);
            msg << "|cffffff00(x" << (castCount-1) << " left)|r";
        }
        ai->TellMasterNoFacing(msg.str());
    }
    else
    {
        msg << "Cast " << spellName.str() << " is failed";
        ai->TellError(msg.str());
    }

    return result;
}


bool CastRandomSpellAction::Execute(Event event)
{
    PlayerSpellMap const& spellMap = bot->GetSpellMap();
    Player* master = GetMaster();

    Unit* target;
    GameObject* got;

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

    vector<pair<uint32, pair<Unit*, GameObject*>>> spellList;

    for (auto & spell : spellMap)
    {
        uint32 spellId = spell.first;

        if (spell.second.state == PLAYERSPELL_REMOVED || spell.second.disabled || IsPassiveSpell(spellId))
            continue;

        const SpellEntry* pSpellInfo = sServerFacade.LookupSpellInfo(spellId);
        if (!pSpellInfo)
            continue;

        if (pSpellInfo->Effect[0] == SPELL_EFFECT_LEARN_SPELL)
            continue;

        if (bot->HasSpell(spellId))
        {
            if (target && ai->CanCastSpell(spellId, target, true))
                spellList.push_back(make_pair(spellId, make_pair(target, nullptr)));
            if (got && ai->CanCastSpell(spellId, got->GetPositionX(), got->GetPositionY(), got->GetPositionZ(), true))
                spellList.push_back(make_pair(spell.first, make_pair(nullptr, got)));
            if (ai->CanCastSpell(spellId, bot, true))
                spellList.push_back(make_pair(spellId, make_pair(bot, nullptr)));
        }
    }

    if (spellList.empty())
        return false;

    bool isCast = false;

    for (uint32 i = 0; i < 5; i++)
    {
        uint32 rnd = urand(0, spellList.size() - 1);

        uint32 spellId = spellList[rnd].first;

        Unit* unit = spellList[rnd].second.first;
        GameObject* go = spellList[rnd].second.second;


        if(spellList[rnd].second.first)
            isCast = ai->CastSpell(spellId, unit);
        else
            isCast = ai->CastSpell(spellId, go->GetPositionX(), go->GetPositionY(), go->GetPositionZ());

        if (isCast)
            return true;
    }

    return false;
}