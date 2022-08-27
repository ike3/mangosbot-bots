#pragma once
#include "botpch.h"
#include "../../playerbot.h"
#include "RpgTriggers.h"
#include "../../PlayerbotAIConfig.h"
#include "../actions/GuildCreateActions.h"
#include "SocialMgr.h"
#include "../../ServerFacade.h"

using namespace ai;

bool RpgTaxiTrigger::IsActive()
{
    GuidPosition guidP(getGuidP());

    if (!guidP.HasNpcFlag(UNIT_NPC_FLAG_FLIGHTMASTER))
        return false;

    uint32 node = sObjectMgr.GetNearestTaxiNode(guidP.getX(), guidP.getY(), guidP.getZ(), guidP.getMapId(), bot->GetTeam());

    if (!node)
        return false;

    if (!bot->m_taxi.IsTaximaskNodeKnown(node))
        return false;

    vector<uint32> nodes;
    for (uint32 i = 0; i < sTaxiPathStore.GetNumRows(); ++i)
    {
        TaxiPathEntry const* entry = sTaxiPathStore.LookupEntry(i);
        if (entry && entry->from == node && (bot->m_taxi.IsTaximaskNodeKnown(entry->to) || bot->isTaxiCheater()))
        {
            return true;
        }
    }

    return false;
}

bool RpgDiscoverTrigger::IsActive()
{
    GuidPosition guidP(getGuidP());

    if (!guidP.HasNpcFlag(UNIT_NPC_FLAG_FLIGHTMASTER))
        return false;

    if (bot->isTaxiCheater())
        return false;

    uint32 node = sObjectMgr.GetNearestTaxiNode(guidP.getX(), guidP.getY(), guidP.getZ(), guidP.getMapId(), bot->GetTeam());

    if (bot->m_taxi.IsTaximaskNodeKnown(node))
        return false;

    return true;
}

bool RpgStartQuestTrigger::IsActive()
{
    GuidPosition guidP(getGuidP());

    if (!guidP.IsCreature() && !guidP.IsGameObject())
        return false;

    if (guidP.IsUnit())
    {
        Unit* unit = guidP.GetUnit();
        if (unit && !unit->HasFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER))
            return false;
    }

    if (AI_VALUE(bool, "can fight equal"))
    {
        if (AI_VALUE2(bool, "can accept quest npc", guidP.GetEntry()))
            return true;
    }
    else
    {
        if (AI_VALUE2(bool, "can accept quest low level npc", guidP.GetEntry()))
            return true;
    }

    return false;
}

bool RpgEndQuestTrigger::IsActive()
{
    GuidPosition guidP(getGuidP());

    if (!guidP.IsCreature() && !guidP.IsGameObject())
        return false;

    if (AI_VALUE2(bool, "can turn in quest npc", guidP.GetEntry()))
        return true;

    return false;
}

bool RpgRepeatQuestTrigger::IsActive()
{
    GuidPosition guidP(getGuidP());

    if (!guidP.IsCreature() && !guidP.IsGameObject())
        return false;

    if (AI_VALUE2(bool, "can repeat quest npc", guidP.GetEntry()))
        return true;

    return false;
}


bool RpgBuyTrigger::IsActive()
{
    GuidPosition guidP(getGuidP());

    if (!guidP.HasNpcFlag(UNIT_NPC_FLAG_VENDOR))
        return false;

    if (AI_VALUE(uint8, "durability") < 50)
        return false;

    if (AI_VALUE(bool, "can sell")) //Need better condition.
        return false;

    return true;
}

bool RpgSellTrigger::IsActive()
{
    GuidPosition guidP(getGuidP());

    if (!guidP.HasNpcFlag(UNIT_NPC_FLAG_VENDOR))
        return false;

    if (!AI_VALUE(bool, "can sell"))
        return false;

    return true;
}

bool RpgAHSellTrigger::IsActive()
{
    GuidPosition guidP(getGuidP());

    if (!guidP.HasNpcFlag(UNIT_NPC_FLAG_AUCTIONEER))
        return false;

    if (!AI_VALUE(bool, "can ah sell"))
        return false;

    return true;
}

bool RpgAHBuyTrigger::IsActive()
{
    GuidPosition guidP(getGuidP());

    if (!guidP.HasNpcFlag(UNIT_NPC_FLAG_AUCTIONEER))
        return false;

    if (AI_VALUE(uint8, "durability") < 50)
        return false;

    if (AI_VALUE(bool, "can ah sell")) //Need better condition.
        return false;

    if (AI_VALUE2(uint32, "free money for", (uint32)NeedMoneyFor::ammo) == 0)
        return false;

    return true;
}

bool RpgGetMailTrigger::IsActive()
{
    GuidPosition guidP(getGuidP());

    if (!guidP.isGoType(GAMEOBJECT_TYPE_MAILBOX))
        return false;

    if (!bot->GetMailSize())
        return false;

    WorldPacket p;
    bot->GetSession()->HandleQueryNextMailTime(p);

    return true;
}

bool RpgRepairTrigger::IsActive()
{
    GuidPosition guidP(getGuidP());

    if (!guidP.HasNpcFlag(UNIT_NPC_FLAG_REPAIR))
        return false;

    if (AI_VALUE2_LAZY(bool, "group or", "should repair,can repair,following party,near leader"))
        return true;

    return false;
}

bool RpgTrainTrigger::IsTrainerOf(CreatureInfo const* cInfo, Player* pPlayer)
{

    switch (cInfo->TrainerType)
    {
    case TRAINER_TYPE_CLASS:
        if (pPlayer->getClass() != cInfo->TrainerClass)
        {
            return false;
        }
        break;
    case TRAINER_TYPE_PETS:
        if (pPlayer->getClass() != CLASS_HUNTER)
        {
            return false;
        }
        break;
    case TRAINER_TYPE_MOUNTS:
        if (cInfo->TrainerRace && pPlayer->getRace() != cInfo->TrainerRace)
        {
            // Allowed to train if exalted
            if (FactionTemplateEntry const* faction_template = sFactionTemplateStore.LookupEntry(cInfo->Faction))
            {
                if (pPlayer->GetReputationRank(faction_template->faction) == REP_EXALTED)
                    return true;
            }
            return false;
        }
        break;
    case TRAINER_TYPE_TRADESKILLS:
        if (cInfo->TrainerSpell && !pPlayer->HasSpell(cInfo->TrainerSpell))
        {
            return false;
        }
        break;
    default:
        return false;                                   // checked and error output at creature_template loading
    }
    return true;
}


bool RpgTrainTrigger::IsActive()
{
    GuidPosition guidP(getGuidP());

    if (!guidP.HasNpcFlag(UNIT_NPC_FLAG_TRAINER))
        return false;

    CreatureInfo const* cInfo = guidP.GetCreatureTemplate();

    if (!IsTrainerOf(cInfo, bot))
        return false;

    // check present spell in trainer spell list
    TrainerSpellData const* cSpells = sObjectMgr.GetNpcTrainerSpells(guidP.GetEntry());

    uint32 trainerId = cInfo->TrainerTemplateId;
    TrainerSpellData const* tSpells = trainerId ? sObjectMgr.GetNpcTrainerTemplateSpells(trainerId) : nullptr;

    if (!cSpells && !tSpells)
    {
        return false;
    }

    FactionTemplateEntry const* factionTemplate = sFactionTemplateStore.LookupEntry(cInfo->Faction);
    float fDiscountMod = bot->GetReputationPriceDiscount(factionTemplate);

    TrainerSpellMap trainer_spells;
    if (cSpells)
        trainer_spells.insert(cSpells->spellList.begin(), cSpells->spellList.end());
    if (tSpells)
        trainer_spells.insert(tSpells->spellList.begin(), tSpells->spellList.end());

    for (TrainerSpellMap::const_iterator itr = trainer_spells.begin(); itr != trainer_spells.end(); ++itr)
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
        if (cost > AI_VALUE2(uint32, "free money for", (uint32)NeedMoneyFor::spells))
            continue;

        return true;
    }
    return false;
}

bool RpgHealTrigger::IsActive()
{
    if (!ai->HasStrategy("heal", BOT_STATE_COMBAT))
        return false;

    GuidPosition guidP(getGuidP());

    Unit* unit = guidP.GetUnit();

    if (!unit)
        return false;

    if (!unit->IsFriend(bot))
        return false;

    if (unit->IsDead() || unit->GetHealthPercent() >= 100)
        return false;

    return true;
}

bool RpgHomeBindTrigger::IsActive()
{
    GuidPosition guidP(getGuidP());

    if (!guidP.HasNpcFlag(UNIT_NPC_FLAG_INNKEEPER))
        return false;

    if (AI_VALUE(WorldPosition, "home bind").distance(bot) < 500.0f)
        return false;

    return true;
}

bool RpgQueueBGTrigger::IsActive()
{
    // skip bots not in continents
    if (!WorldPosition(bot).isOverworld()) // bg, raid, dungeon
        return false;

    GuidPosition guidP(getGuidP());

    if (!guidP.IsCreature())
        return false;

    if (AI_VALUE(BattleGroundTypeId, "rpg bg type") == BATTLEGROUND_TYPE_NONE)
        return false;

    return true;
}

bool RpgBuyPetitionTrigger::IsActive()
{
    GuidPosition guidP(getGuidP());

    if (!guidP.HasNpcFlag(UNIT_NPC_FLAG_PETITIONER))
        return false;

    if (!BuyPetitionAction::canBuyPetition(bot))
        return false;

    return true;
}

bool RpgUseTrigger::IsActive()
{
    if (ai->HasRealPlayerMaster())
        return false;

    GuidPosition guidP(getGuidP());

    if (!guidP.IsGameObject())
        return false;

    GameObjectInfo const* goInfo = guidP.GetGameObjectInfo();

    switch (goInfo->type) {
    case GAMEOBJECT_TYPE_DOOR:
    case    GAMEOBJECT_TYPE_BUTTON:
    case GAMEOBJECT_TYPE_QUESTGIVER:
    case GAMEOBJECT_TYPE_CHEST:
    case GAMEOBJECT_TYPE_TRAP:
    case GAMEOBJECT_TYPE_CHAIR:
    case GAMEOBJECT_TYPE_SPELL_FOCUS:
    case GAMEOBJECT_TYPE_SUMMONING_RITUAL:
    case GAMEOBJECT_TYPE_MAILBOX:
    case GAMEOBJECT_TYPE_MEETINGSTONE:
    case GAMEOBJECT_TYPE_FLAGSTAND:
    case GAMEOBJECT_TYPE_FLAGDROP:
    case GAMEOBJECT_TYPE_MINI_GAME:
    case GAMEOBJECT_TYPE_CAPTURE_POINT:
#ifndef MANGOSBOT_TWO
    case GAMEOBJECT_TYPE_AUCTIONHOUSE:
    case GAMEOBJECT_TYPE_LOTTERY_KIOSK:
#endif
        return true;
    case GAMEOBJECT_TYPE_BINDER:
    case GAMEOBJECT_TYPE_GENERIC:
    case GAMEOBJECT_TYPE_TEXT:
    case GAMEOBJECT_TYPE_GOOBER:
    case GAMEOBJECT_TYPE_TRANSPORT:
    case GAMEOBJECT_TYPE_AREADAMAGE:
    case GAMEOBJECT_TYPE_CAMERA:
    case GAMEOBJECT_TYPE_MAP_OBJECT:
    case GAMEOBJECT_TYPE_MO_TRANSPORT:
    case GAMEOBJECT_TYPE_DUEL_ARBITER:
    case GAMEOBJECT_TYPE_FISHINGNODE:
    case GAMEOBJECT_TYPE_GUARDPOST:
    case GAMEOBJECT_TYPE_SPELLCASTER:
    case GAMEOBJECT_TYPE_FISHINGHOLE:
    case GAMEOBJECT_TYPE_AURA_GENERATOR:
        return false;
    }

    return true;
}

bool RpgSpellTrigger::IsActive()
{
    GuidPosition guidP(getGuidP());

    if (guidP.IsPlayer())
        return false;

    if (!guidP.GetWorldObject())
        return false;

    return true;
}

bool RpgCraftTrigger::IsActive()
{
    GuidPosition guidP(getGuidP());

    if (guidP.IsPlayer())
        return false;

    if (!guidP.GetWorldObject())
        return false;

    return true;
}

bool RpgTradeUsefulTrigger::isFriend(Player* player)
{
    if (ai->IsAlt() && GetMaster() == player)
        return true;

    if (player->GetPlayerbotAI() && player->GetPlayerbotAI()->GetMaster() == bot && player->GetPlayerbotAI()->IsAlt())
        return true;

    if (player->GetGuildId() && player->GetGuildId() == bot->GetGuildId())
        return true;

    if (bot->GetGroup() == player->GetGroup() && !urand(0, 5))
        return true;

    if (!urand(0, 20))
        return true;

    return false;
}

bool RpgTradeUsefulTrigger::IsActive()
{
    GuidPosition guidP(getGuidP());

    if (!guidP.IsPlayer())
        return false;

    Player* player = guidP.GetPlayer();


    if (!player)
        return false;

    if (player->GetTrader() == bot && bot->GetTrader() == player) //Continue trading please.
        return true;

    if (!isFriend(player))
        return false;

    if (!player->IsWithinLOSInMap(bot))
        return false;

    //Trading with someone else
    if (player->GetTrader() && player->GetTrader() != bot)
        return false;

    if (bot->GetTrader() && bot->GetTrader() != player)
        return false;

    if (AI_VALUE(list<Item*>, "items useful to give").empty())
        return false;

    return true;
}

bool RpgDuelTrigger::IsActive()
{
    if(!ai->HasStrategy("start duel", BOT_STATE_NON_COMBAT))
        return false;

    // Less spammy duels
    if (bot->GetLevel() < 3)
        return false;

    if (ai->HasRealPlayerMaster())
    {
        // do not auto duel if master is not afk
        if (ai->GetMaster() && !ai->GetMaster()->isAFK())
            return false;
    }

    // do not auto duel with low hp
    if (AI_VALUE2(uint8, "health", "self target") < 90)
        return false;

    GuidPosition guidP(getGuidP());

    if (!guidP.IsPlayer())
        return false;

    Player* player = guidP.GetPlayer();

    if (!player)
        return false;

    if (player->GetLevel() > bot->GetLevel() + 3)
        return false;

    if (bot->GetLevel() > player->GetLevel() + 10)
        return false;

    // caster or target already have requested duel
    if (bot->duel || player->duel || !player->GetSocial() || player->GetSocial()->HasIgnore(bot->GetObjectGuid()))
        return false;

    AreaTableEntry const* targetAreaEntry = GetAreaEntryByAreaID(sServerFacade.GetAreaId(player));
    if (targetAreaEntry && !(targetAreaEntry->flags & AREA_FLAG_DUEL))
    {
        // Dueling isn't allowed here
        return false;
    }

    if (!AI_VALUE(list<ObjectGuid>, "all targets").empty())
        return false;

    return true;
}
