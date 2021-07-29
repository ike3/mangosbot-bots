#include "botpch.h"
#include "../../playerbot.h"
#include "RpgAction.h"
#include "ChooseRpgTargetAction.h"
#include "../../PlayerbotAIConfig.h"
#include "../values/PossibleRpgTargetsValue.h"
#include "EmoteAction.h"
#include "GossipDef.h"


using namespace ai;

bool RpgAction::Execute(Event event)
{    
    ObjectGuid guid = AI_VALUE(ObjectGuid, "rpg target");

    if (!guid && ai->GetMaster())
    {
        guid = ai->GetMaster()->GetSelectionGuid();
        if (guid)
        {
            RemIgnore(guid);
            context->GetValue<ObjectGuid>("rpg target")->Set(guid);
        }
    }


    WorldObject* wo = ai->GetWorldObject(guid);
    Unit* unit = ai->GetUnit(guid);
    GameObject* go = ai->GetGameObject(guid);
    if (!wo)
    {
        context->GetValue<ObjectGuid>("rpg target")->Set(ObjectGuid());
        return false;
    }

    if (sServerFacade.isMoving(bot))
        return true;

    if (bot->GetMapId() != wo->GetMapId())
    {
        context->GetValue<ObjectGuid>("rpg target")->Set(ObjectGuid());
        return false;
    }

    if (!sServerFacade.IsInFront(bot, wo, sPlayerbotAIConfig.sightDistance, CAST_ANGLE_IN_FRONT) && !bot->IsTaxiFlying() && !bot->IsFlying())
    {
        sServerFacade.SetFacingTo(bot, wo, true);
        ai->SetNextCheckDelay(sPlayerbotAIConfig.globalCoolDown);
        return true;
    }

    if (unit && !bot->GetNPCIfCanInteractWith(guid, UNIT_NPC_FLAG_NONE))
    {
        context->GetValue<ObjectGuid>("rpg target")->Set(ObjectGuid());
        return false;
    }

    if (bot->GetShapeshiftForm() > 0)
        bot->SetShapeshiftForm(FORM_NONE);

    //Random taxi action.
    if (unit && unit->HasFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_FLIGHTMASTER) && !ai->HasRealPlayerMaster())
    {
        WorldPacket emptyPacket;
        bot->GetSession()->HandleCancelMountAuraOpcode(emptyPacket);
        taxi(guid);
        return true;
    }
   
    vector<RpgElement> elements;

    uint32 dStatus = bot->GetSession()->getDialogStatus(bot, wo, DIALOG_STATUS_NONE);

#ifdef MANGOSBOT_ZERO  
    if (dStatus == DIALOG_STATUS_REWARD2 || dStatus == DIALOG_STATUS_REWARD_REP || dStatus == DIALOG_STATUS_AVAILABLE || (AI_VALUE(uint8, "durability") <= 20 || dStatus == DIALOG_STATUS_CHAT))
#endif
#ifdef MANGOSBOT_ONE
    if (dStatus == DIALOG_STATUS_REWARD2 || dStatus == DIALOG_STATUS_REWARD || dStatus == DIALOG_STATUS_REWARD_REP || dStatus == DIALOG_STATUS_AVAILABLE || (AI_VALUE(uint8, "durability") <= 20 || dStatus == DIALOG_STATUS_CHAT))
#endif
#ifdef MANGOSBOT_TWO
    if (dStatus == DIALOG_STATUS_REWARD2 || dStatus == DIALOG_STATUS_REWARD || dStatus == DIALOG_STATUS_REWARD_REP || dStatus == DIALOG_STATUS_AVAILABLE || (AI_VALUE(uint8, "durability") <= 20 || dStatus == DIALOG_STATUS_LOW_LEVEL_AVAILABLE))
#endif  
        elements.push_back(&RpgAction::quest);
    if (unit)
    {
        if (unit->isVendor())
            elements.push_back(&RpgAction::trade);
        if (unit->isArmorer() && (AI_VALUE(uint8, "durability") < 100 && AI_VALUE(uint32, "repair cost") < bot->GetMoney()))
            elements.push_back(&RpgAction::repair);
        if (CanTrain(guid))
            elements.push_back(&RpgAction::train);
        if (unit->GetHealthPercent() < 100 && (bot->getClass() == CLASS_PRIEST || bot->getClass() == CLASS_DRUID || bot->getClass() == CLASS_PALADIN || bot->getClass() == CLASS_SHAMAN))
            elements.push_back(&RpgAction::heal);
        if (unit->isInnkeeper() && AI_VALUE2(float, "distance", "home bind") > 1000.0f)
            elements.push_back(&RpgAction::homebind);
        if (unit->isBattleMaster() && CanQueueBg(guid))
            elements.push_back(&RpgAction::queuebg);
    }
    else
    {
        if (!go->IsInUse() && go->GetGoState() == GO_STATE_READY)
        {
            elements.push_back(&RpgAction::use);
            elements.push_back(&RpgAction::work);
            elements.push_back(&RpgAction::spell);
            elements.push_back(&RpgAction::craft);
        }
    }

    if (AddIgnore(guid))
    {
        if (elements.empty() && ChooseRpgTargetAction::isFollowValid(bot, wo))
        {
            elements.push_back(&RpgAction::emote);
            elements.push_back(&RpgAction::stay);
            elements.push_back(&RpgAction::work);
            elements.push_back(&RpgAction::spell);
            elements.push_back(&RpgAction::craft);
        }
    }    
    else
        elements.push_back(&RpgAction::cancel);

    if (elements.empty())
        elements.push_back(&RpgAction::cancel);

    RpgElement element = elements[urand(0, elements.size() - 1)];
    (this->*element)(guid);
    return true;
}

bool RpgAction::AddIgnore(ObjectGuid guid)
{
    if (HasIgnore(guid))
        return false;

    set<ObjectGuid>& ignoreList = context->GetValue<set<ObjectGuid>&>("ignore rpg target")->Get();

    ignoreList.insert(guid);

    if (ignoreList.size() > 50)
        ignoreList.erase(ignoreList.begin());

    context->GetValue<set<ObjectGuid>&>("ignore rpg target")->Set(ignoreList);

    return true;
}

bool RpgAction::RemIgnore(ObjectGuid guid)
{
    if (!HasIgnore(guid))
        return false;

    set<ObjectGuid>& ignoreList = context->GetValue<set<ObjectGuid>&>("ignore rpg target")->Get();

    ignoreList.erase(ignoreList.find(guid));

    context->GetValue<set<ObjectGuid>&>("ignore rpg target")->Set(ignoreList);

    return true;
}

bool RpgAction::HasIgnore(ObjectGuid guid)
{
    set<ObjectGuid>& ignoreList = context->GetValue<set<ObjectGuid>&>("ignore rpg target")->Get();
    if (ignoreList.empty())
        return false;

    if (ignoreList.find(guid) == ignoreList.end())
        return false;

    return true;
}

void RpgAction::stay(ObjectGuid guid)
{
    if (bot->GetPlayerMenu()) bot->GetPlayerMenu()->CloseGossip();
    ai->SetNextCheckDelay(sPlayerbotAIConfig.rpgDelay);
}

void RpgAction::work(ObjectGuid guid)
{
    bot->HandleEmoteCommand(EMOTE_STATE_USESTANDING);
    ai->SetNextCheckDelay(sPlayerbotAIConfig.rpgDelay);
}

void RpgAction::emote(ObjectGuid guid)
{
    Unit* unit = ai->GetUnit(guid);
    uint32 type = TalkAction::GetRandomEmote(unit);

    ObjectGuid oldSelection = bot->GetSelectionGuid();

    bot->SetSelectionGuid(guid);

    WorldPacket p1;
    p1 << guid;
    bot->GetSession()->HandleGossipHelloOpcode(p1);

    bot->HandleEmoteCommand(type);

    if (unit)
        unit->SetFacingTo(unit->GetAngle(bot));

    if (oldSelection)
        bot->SetSelectionGuid(oldSelection);

    ai->SetNextCheckDelay(sPlayerbotAIConfig.rpgDelay);
}

void RpgAction::cancel(ObjectGuid guid)
{
    context->GetValue<ObjectGuid>("rpg target")->Set(ObjectGuid());
}

void RpgAction::taxi(ObjectGuid guid)
{
    Unit* unit = ai->GetUnit(guid);
    uint32 curloc = sObjectMgr.GetNearestTaxiNode(unit->GetPositionX(), unit->GetPositionY(), unit->GetPositionZ(), unit->GetMapId(), bot->GetTeam());

    vector<uint32> nodes;
    for (uint32 i = 0; i < sTaxiPathStore.GetNumRows(); ++i)
    {
        TaxiPathEntry const* entry = sTaxiPathStore.LookupEntry(i);
        if (entry && entry->from == curloc)
        {
            //uint8  field = uint8((i - 1) / 32);
            //if (field < TaxiMaskSize) nodes.push_back(i);
			nodes.push_back(i);
        }
    }

    if (nodes.empty())
    {
        sLog.outError("Bot %s - No flight paths available", bot->GetName());
        return;
    }

    context->GetValue<ObjectGuid>("rpg target")->Set(ObjectGuid());

    uint32 path = nodes[urand(0, nodes.size() - 1)];
    //bot->m_taxi.SetTaximaskNode(path); // Causes crash/freeze
    uint32 money = bot->GetMoney();
    bot->SetMoney(money + 100000);

    TaxiPathEntry const* entry = sTaxiPathStore.LookupEntry(path);
    if (!entry)
        return;

    TaxiNodesEntry const* nodeFrom = sTaxiNodesStore.LookupEntry(entry->from);
    TaxiNodesEntry const* nodeTo = sTaxiNodesStore.LookupEntry(entry->to);

    Creature* flightMaster = bot->GetNPCIfCanInteractWith(guid, UNIT_NPC_FLAG_FLIGHTMASTER);
    if (!flightMaster)
    {
        sLog.outError("Bot %s cannot talk to flightmaster (%zu location available)", bot->GetName(), nodes.size());
        return;
    }
    if (!bot->ActivateTaxiPathTo({ entry->from, entry->to }, flightMaster, 0))
    {
        sLog.outError("Bot %s cannot fly %u (%zu location available)", bot->GetName(), path, nodes.size());
        return;
    }
    sLog.outString("Bot #%d <%s> is flying from %s to %s (%zu location available)", bot->GetGUIDLow(), bot->GetName(), nodeFrom->name[0], nodeTo->name[0], nodes.size());
    bot->SetMoney(money);
}

void RpgAction::quest(ObjectGuid guid)
{
    WorldObject* wo = ai->GetWorldObject(guid);
    uint32 type = TalkAction::GetRandomEmote(ai->GetUnit(guid));

    ObjectGuid oldSelection = bot->GetSelectionGuid();

    bot->SetSelectionGuid(guid);      

    //Parse rpg target to quest action.
    WorldPacket p(CMSG_QUESTGIVER_ACCEPT_QUEST);
    p << guid;
    p.rpos(0);

    bool retVal = false;

    uint32 dStatus = bot->GetSession()->getDialogStatus(bot, wo, DIALOG_STATUS_NONE);

#ifndef MANGOSBOT_TWO
    if (dStatus == DIALOG_STATUS_AVAILABLE || (AI_VALUE(uint8, "durability") <= 20 || dStatus == DIALOG_STATUS_CHAT))
#else
    if (dStatus == DIALOG_STATUS_AVAILABLE || (AI_VALUE(uint8, "durability") <= 20 || dStatus == DIALOG_STATUS_LOW_LEVEL_AVAILABLE))
#endif    
        retVal = ai->DoSpecificAction("accept all quests", Event("rpg action", p));
#ifdef MANGOSBOT_ZERO
    if (dStatus == DIALOG_STATUS_REWARD2 || dStatus == DIALOG_STATUS_REWARD_REP)
#else
    if (dStatus == DIALOG_STATUS_REWARD2 || dStatus == DIALOG_STATUS_REWARD || dStatus == DIALOG_STATUS_REWARD_REP)
#endif
        retVal = ai->DoSpecificAction("talk to quest giver", Event("rpg action", p));
    
    if(!retVal)
        bot->HandleEmoteCommand(type);

    if (retVal)
        RemIgnore(guid);

    bot->HandleEmoteCommand(type);

    Unit* unit = ai->GetUnit(guid);
    if (unit)
        unit->SetFacingTo(unit->GetAngle(bot));

    if (oldSelection)
        bot->SetSelectionGuid(oldSelection);

    //Speed up if 
    if (!ai->HasRealPlayerMaster())
        ai->SetNextCheckDelay(sPlayerbotAIConfig.rpgDelay);

    cancel(guid);
}

void RpgAction::trade(ObjectGuid guid)
{
    ObjectGuid oldSelection = bot->GetSelectionGuid();

    bot->SetSelectionGuid(guid);

    ai->DoSpecificAction("sell", Event("rpg action", "vendor"));

    if(AI_VALUE(uint8, "durability") > 50)
        ai->DoSpecificAction("buy", Event("rpg action", "vendor"));

    Unit* unit = ai->GetUnit(guid);
    if (unit)
        unit->SetFacingTo(unit->GetAngle(bot));

    if (oldSelection)
        bot->SetSelectionGuid(oldSelection);

    if (!ai->HasRealPlayerMaster())
        ai->SetNextCheckDelay(sPlayerbotAIConfig.rpgDelay);
}

void RpgAction::repair(ObjectGuid guid)
{
    ObjectGuid oldSelection = bot->GetSelectionGuid();

    bot->SetSelectionGuid(guid);

    ai->DoSpecificAction("repair");

    Unit* unit = ai->GetUnit(guid);
    if (unit)
        unit->SetFacingTo(unit->GetAngle(bot));

    if (oldSelection)
        bot->SetSelectionGuid(oldSelection);

    if (!ai->HasRealPlayerMaster())
        ai->SetNextCheckDelay(sPlayerbotAIConfig.rpgDelay);
}

void RpgAction::train(ObjectGuid guid)
{
    ObjectGuid oldSelection = bot->GetSelectionGuid();

    bot->SetSelectionGuid(guid);

    ai->DoSpecificAction("trainer");

    Unit* unit = ai->GetUnit(guid);
    if (unit)
        unit->SetFacingTo(unit->GetAngle(bot));

    if (oldSelection)
        bot->SetSelectionGuid(oldSelection);

    if (!ai->HasRealPlayerMaster())
        ai->SetNextCheckDelay(sPlayerbotAIConfig.rpgDelay);
}

void RpgAction::heal(ObjectGuid guid)
{
    ObjectGuid oldSelection = bot->GetSelectionGuid();

    bot->SetSelectionGuid(guid);

    switch (bot->getClass())
    {
    case CLASS_PRIEST:
        ai->DoSpecificAction("lesser heal on party");
        /* Example to cast a specific spel on a specif unit. Maybe usefull later.
        if(!ai->HasAura("power word: fortitude", unit))
            ai->DoSpecificAction("cast custom spell", Event("rpg action", chat->formatWorldobject(unit) + " power word: fortitude"), true);
        else
            ai->DoSpecificAction("cast custom spell", Event("rpg action", chat->formatWorldobject(unit) + " 2052"), true); 
        */
        break;
    case CLASS_DRUID:
        ai->DoSpecificAction("healing touch on party");
        break;
    case CLASS_PALADIN:
        ai->DoSpecificAction("holy light on party");
        break;
    case CLASS_SHAMAN:
        ai->DoSpecificAction("healing wave on party");
        break;
    }

    Unit* unit = ai->GetUnit(guid);
    if (unit)
        unit->SetFacingTo(unit->GetAngle(bot));

    if (oldSelection)
        bot->SetSelectionGuid(oldSelection);

    if (!ai->HasRealPlayerMaster())
        ai->SetNextCheckDelay(sPlayerbotAIConfig.rpgDelay);
}

void RpgAction::use(ObjectGuid guid)
{
    ObjectGuid oldSelection = bot->GetSelectionGuid();

    bot->SetSelectionGuid(guid);

    WorldObject* wo = ai->GetWorldObject(guid);

    ai->DoSpecificAction("use", Event("rpg action", chat->formatWorldobject(wo)));

    if (oldSelection)
        bot->SetSelectionGuid(oldSelection);

    if (!ai->HasRealPlayerMaster())
        ai->SetNextCheckDelay(sPlayerbotAIConfig.rpgDelay);
}

void RpgAction::spell(ObjectGuid guid)
{
    ObjectGuid oldSelection = bot->GetSelectionGuid();

    bot->SetSelectionGuid(guid);

    WorldObject* wo = ai->GetWorldObject(guid);

    ai->DoSpecificAction("cast random spell", Event("rpg action", chat->formatWorldobject(wo)));

    Unit* unit = ai->GetUnit(guid);
    if (unit)
        unit->SetFacingTo(unit->GetAngle(bot));

    if (oldSelection)
        bot->SetSelectionGuid(oldSelection);

    if (!ai->HasRealPlayerMaster())
        ai->SetNextCheckDelay(sPlayerbotAIConfig.rpgDelay);
}

void RpgAction::craft(ObjectGuid guid)
{
    ObjectGuid oldSelection = bot->GetSelectionGuid();

    bot->SetSelectionGuid(guid);

    WorldObject* wo = ai->GetWorldObject(guid);

    bool crafted = ai->DoSpecificAction("craft random item", Event("rpg action", chat->formatWorldobject(wo)));

    Unit* unit = ai->GetUnit(guid);
    if (unit)
        unit->SetFacingTo(unit->GetAngle(bot));

    if (oldSelection)
        bot->SetSelectionGuid(oldSelection);

    if (crafted)
        RemIgnore(guid);

    if (!ai->HasRealPlayerMaster())
        ai->SetNextCheckDelay(sPlayerbotAIConfig.rpgDelay);
}

void RpgAction::homebind(ObjectGuid guid)
{
    ObjectGuid oldSelection = bot->GetSelectionGuid();

    bot->SetSelectionGuid(guid);

    ai->DoSpecificAction("home");

    Unit* unit = ai->GetUnit(guid);
    if (unit)
        unit->SetFacingTo(unit->GetAngle(bot));

    if (oldSelection)
        bot->SetSelectionGuid(oldSelection);

    if (!ai->HasRealPlayerMaster())
        ai->SetNextCheckDelay(sPlayerbotAIConfig.rpgDelay);
}

void RpgAction::queuebg(ObjectGuid guid)
{
    ObjectGuid oldSelection = bot->GetSelectionGuid();

    bot->SetSelectionGuid(guid);

    BattleGroundTypeId bgTypeId = CanQueueBg(guid);

    bot->GetPlayerbotAI()->GetAiObjectContext()->GetValue<uint32>("bg type")->Set((uint32)bgTypeId);
    ai->DoSpecificAction("free bg join");

    Unit* unit = ai->GetUnit(guid);
    if (unit)
        unit->SetFacingTo(unit->GetAngle(bot));

    if (oldSelection)
        bot->SetSelectionGuid(oldSelection);

    if (!ai->HasRealPlayerMaster())
        ai->SetNextCheckDelay(sPlayerbotAIConfig.rpgDelay);
}

bool RpgAction::isUseful()
{
    return context->GetValue<ObjectGuid>("rpg target")->Get();
}


bool RpgAction::CanTrain(ObjectGuid guid)
{
    Creature* creature = ai->GetCreature(guid);

    if (!creature)
        return false;

    if (!creature->IsTrainerOf(bot, false))
        return false;

    // check present spell in trainer spell list
    TrainerSpellData const* cSpells = creature->GetTrainerSpells();
    TrainerSpellData const* tSpells = creature->GetTrainerTemplateSpells();
    if (!cSpells && !tSpells)
    {
        return false;
    }

    float fDiscountMod = bot->GetReputationPriceDiscount(creature);

    TrainerSpellData const* trainer_spells = cSpells;
    if (!trainer_spells)
        trainer_spells = tSpells;

    for (TrainerSpellMap::const_iterator itr = trainer_spells->spellList.begin(); itr != trainer_spells->spellList.end(); ++itr)
    {
        TrainerSpell const* tSpell = &itr->second;

        if (!tSpell)
            continue;

        uint32 reqLevel = 0;

        reqLevel = tSpell->isProvidedReqLevel ? tSpell->reqLevel : std::max(reqLevel, tSpell->reqLevel);
        TrainerSpellState state = bot->GetTrainerSpellState(tSpell, reqLevel);
        if (state != TRAINER_SPELL_GREEN)
            continue;

        uint32 spellId = tSpell->spell;
        const SpellEntry* const pSpellInfo = sServerFacade.LookupSpellInfo(spellId);
        if (!pSpellInfo)
            continue;

        uint32 cost = uint32(floor(tSpell->spellCost * fDiscountMod));
        if (cost > bot->GetMoney())
            continue;

        return true;
    }

    return false;
}

BattleGroundTypeId RpgAction::CanQueueBg(ObjectGuid guid)
{
    for (uint32 i = 1; i < MAX_BATTLEGROUND_QUEUE_TYPES; i++)
    {
        BattleGroundQueueTypeId queueTypeId = (BattleGroundQueueTypeId)i;

        BattleGroundTypeId bgTypeId = sServerFacade.BgTemplateId(queueTypeId);

        BattleGround* bg = sBattleGroundMgr.GetBattleGroundTemplate(bgTypeId);
        if (!bg)
            continue;

        if (bot->getLevel() < bg->GetMinLevel())
            continue;

        // check if already in queue
        if (bot->InBattleGroundQueueForBattleGroundQueueType(queueTypeId))
            continue;

        map<Team, map<BattleGroundTypeId, list<uint32>>> battleMastersCache = sRandomPlayerbotMgr.getBattleMastersCache();

        for (auto& entry : battleMastersCache[TEAM_BOTH_ALLOWED][bgTypeId])
            if (entry == guid.GetEntry())
                return bgTypeId;

        for (auto& entry : battleMastersCache[bot->GetTeam()][bgTypeId])
            if (entry == guid.GetEntry())
                return bgTypeId;
    }

    return BATTLEGROUND_TYPE_NONE;
}

