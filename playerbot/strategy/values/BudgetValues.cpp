#include "botpch.h"
#include "../../playerbot.h"
#include "BudgetValues.h"

using namespace ai;

uint32 MaxGearRepairCostValue::Calculate()
{
    uint32 TotalCost = 0;
    for (int i = EQUIPMENT_SLOT_START; i < INVENTORY_SLOT_ITEM_END; ++i)
    {
        uint16 pos = ((INVENTORY_SLOT_BAG_0 << 8) | i);
        Item* item = bot->GetItemByPos(pos);

        if (!item)
            continue;

        uint32 maxDurability = item->GetUInt32Value(ITEM_FIELD_MAXDURABILITY);
        if (!maxDurability)
            continue;

        uint32 curDurability = item->GetUInt32Value(ITEM_FIELD_DURABILITY);

        if (i >= EQUIPMENT_SLOT_END && curDurability >= maxDurability) //Only count items equiped or already damanged.
            continue;

        ItemPrototype const* ditemProto = item->GetProto();

        DurabilityCostsEntry const* dcost = sDurabilityCostsStore.LookupEntry(ditemProto->ItemLevel);
        if (!dcost)
            continue;

        uint32 dQualitymodEntryId = (ditemProto->Quality + 1) * 2;
        DurabilityQualityEntry const* dQualitymodEntry = sDurabilityQualityStore.LookupEntry(dQualitymodEntryId);
        if (!dQualitymodEntry)
            continue;

        uint32 dmultiplier = dcost->multiplier[ItemSubClassToDurabilityMultiplierId(ditemProto->Class, ditemProto->SubClass)];

        uint32 costs = uint32(maxDurability * dmultiplier * double(dQualitymodEntry->quality_mod));


        TotalCost += costs;
    }

    return TotalCost;
}

uint32 RepairCostValue::Calculate()
{
    uint32 TotalCost = 0;
    for (int i = EQUIPMENT_SLOT_START; i < INVENTORY_SLOT_ITEM_END; ++i)
    {
        uint16 pos = ((INVENTORY_SLOT_BAG_0 << 8) | i);
        Item* item = bot->GetItemByPos(pos);

        if (!item)
            continue;

        uint32 maxDurability = item->GetUInt32Value(ITEM_FIELD_MAXDURABILITY);
        if (!maxDurability)
            continue;

        uint32 curDurability = item->GetUInt32Value(ITEM_FIELD_DURABILITY);

        uint32 LostDurability = maxDurability - curDurability;

        if (LostDurability == 0)
            continue;

        ItemPrototype const* ditemProto = item->GetProto();

        DurabilityCostsEntry const* dcost = sDurabilityCostsStore.LookupEntry(ditemProto->ItemLevel);
        if (!dcost)
            continue;

        uint32 dQualitymodEntryId = (ditemProto->Quality + 1) * 2;
        DurabilityQualityEntry const* dQualitymodEntry = sDurabilityQualityStore.LookupEntry(dQualitymodEntryId);
        if (!dQualitymodEntry)
            continue;

        uint32 dmultiplier = dcost->multiplier[ItemSubClassToDurabilityMultiplierId(ditemProto->Class, ditemProto->SubClass)];
        uint32 costs = uint32(LostDurability * dmultiplier * double(dQualitymodEntry->quality_mod));

        TotalCost += costs;
    }

    return TotalCost;
}

uint32 TrainCostValue::Calculate()
{
    uint32 TotalCost = 0;

    set<uint32> spells;

    for (uint32 id = 0; id < sCreatureStorage.GetMaxEntry(); ++id)
    {
        CreatureInfo const* co = sCreatureStorage.LookupEntry<CreatureInfo>(id);
        if (!co)
            continue;

        if (co->TrainerType != TRAINER_TYPE_CLASS && co->TrainerType != TRAINER_TYPE_TRADESKILLS)
            continue;

        if (co->TrainerType == TRAINER_TYPE_CLASS && co->TrainerClass != bot->getClass())
            continue;

        uint32 trainerId = co->TrainerTemplateId;
        if (!trainerId)
            trainerId = co->Entry;

        TrainerSpellData const* trainer_spells = sObjectMgr.GetNpcTrainerTemplateSpells(trainerId);
        if (!trainer_spells)
            trainer_spells = sObjectMgr.GetNpcTrainerSpells(trainerId);

        if (!trainer_spells)
            continue;

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

            if (co->TrainerType == TRAINER_TYPE_TRADESKILLS)
                continue;

            if (spells.find(tSpell->spell) != spells.end())
                continue;

            TotalCost += tSpell->spellCost;
            spells.insert(tSpell->spell);
        }
    }
    return TotalCost;
}

uint32 MoneyNeededForValue::Calculate()
{
	NeedMoneyFor needMoneyFor = NeedMoneyFor(stoi(getQualifier()));

	PlayerbotAI* ai = bot->GetPlayerbotAI();
	AiObjectContext* context = ai->GetAiObjectContext();

	uint32 moneyWanted = 0;

	uint32 level = bot->getLevel();

	switch (needMoneyFor)
	{
	case NeedMoneyFor::none:
		moneyWanted = 0;
		break;
	case NeedMoneyFor::repair:
		moneyWanted = AI_VALUE(uint32, "max repair cost");
		break;
    case NeedMoneyFor::ammo:
        moneyWanted = (bot->getClass() == CLASS_HUNTER) ? (level * level * level) / 10 : 0; //Or level^3 (1s @ lvl10, 30s @ lvl30, 2g @ lvl60, 5g @ lvl80): Todo replace
        break;
	case NeedMoneyFor::spells:
		moneyWanted = AI_VALUE(uint32, "train cost");
		break;
	case NeedMoneyFor::travel:
		moneyWanted = bot->isTaxiCheater() ? 0 : 1500; //15s for traveling half a continent. Todo: Add better calculation
		break;
	case NeedMoneyFor::gear:
		moneyWanted = level * level * level; //Or level^3 (10s @ lvl10, 3g @ lvl30, 20g @ lvl60, 50g @ lvl80): Todo replace
		break;
    case NeedMoneyFor::consumables:
        moneyWanted = (level * level * level)/10; //Or level^3 (1s @ lvl10, 30s @ lvl30, 2g @ lvl60, 5g @ lvl80): Todo replace
        break;
	}    

	return moneyWanted;
};

uint32 TotalMoneyNeededForValue::Calculate()
{
    NeedMoneyFor needMoneyFor = NeedMoneyFor(stoi(getQualifier()));

    uint32 moneyWanted = AI_VALUE2(uint32, "money needed for", (uint32)needMoneyFor);

    auto needPtr = std::find(saveMoneyFor.begin(), saveMoneyFor.end(), needMoneyFor);

    while (needPtr != saveMoneyFor.begin())
    {
        needPtr--;

        NeedMoneyFor alsoNeed = *needPtr;

        moneyWanted = moneyWanted + AI_VALUE2(uint32, "money needed for", (uint32)alsoNeed);
    }

    return moneyWanted;
}

uint32 FreeMoneyForValue::Calculate() 
{
    uint32 money = bot->GetMoney();

    if (ai->HasCheat(BotCheatMask::gold))
        return 10000000;

    if (ai->HasActivePlayerMaster())
        return money;

    uint32 savedMoney = AI_VALUE2(uint32, "total money needed for", getQualifier()) - AI_VALUE2(uint32, "money needed for", getQualifier());

    if (savedMoney > money)
        return 0;

    return money - savedMoney;
};