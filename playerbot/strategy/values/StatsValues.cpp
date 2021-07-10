#include "botpch.h"
#include "../../playerbot.h"
#include "StatsValues.h"

#include "../../ServerFacade.h"
using namespace ai;

uint8 HealthValue::Calculate()
{
    Unit* target = GetTarget();
    if (!target)
        return 100;
    return (static_cast<float> (target->GetHealth()) / target->GetMaxHealth()) * 100;
}

bool IsDeadValue::Calculate()
{
    Unit* target = GetTarget();
    if (!target)
        return false;
    return sServerFacade.GetDeathState(target) != ALIVE;
}

bool PetIsDeadValue::Calculate()
{
#ifdef MANGOSBOT_ZERO
#ifdef MANGOS
    PetDatabaseStatus status = Pet::GetStatusFromDB(bot);
    if (status == PET_DB_DEAD)
#endif
#endif
    if (!bot->GetPet())
    {
        uint32 ownerid = bot->GetGUIDLow();
        QueryResult* result = CharacterDatabase.PQuery("SELECT id FROM character_pet WHERE owner = '%u'", ownerid);
        if (!result)
            return false;

        delete result;
        return true;
    }
    if (bot->GetPetGuid() && !bot->GetPet())
        return true;

    return bot->GetPet() && sServerFacade.GetDeathState(bot->GetPet()) != ALIVE;
}

bool PetIsHappyValue::Calculate()
{
#ifdef MANGOSBOT_ZERO
#ifndef CMANGOS
    PetDatabaseStatus status = Pet::GetStatusFromDB(bot);
    if (status == PET_DB_DEAD)
        return true;
#endif
#endif

    return !bot->GetPet() || bot->GetPet()->GetHappinessState() == HAPPY;
}


uint8 RageValue::Calculate()
{
    Unit* target = GetTarget();
    if (!target)
        return 0;
    return (static_cast<float> (target->GetPower(POWER_RAGE)));
}

uint8 EnergyValue::Calculate()
{
    Unit* target = GetTarget();
    if (!target)
        return 0;
    return (static_cast<float> (target->GetPower(POWER_ENERGY)));
}

uint8 ManaValue::Calculate()
{
    Unit* target = GetTarget();
    if (!target)
        return 100;
    return (static_cast<float> (target->GetPower(POWER_MANA)) / target->GetMaxPower(POWER_MANA)) * 100;
}

bool HasManaValue::Calculate()
{
    Unit* target = GetTarget();
    if (!target)
        return false;
    return target->GetPower(POWER_MANA);
}


uint8 ComboPointsValue::Calculate()
{
    Unit *target = GetTarget();
	if (!target || target->GetObjectGuid() != bot->GetComboTargetGuid())
		return 0;

    return bot->GetComboPoints();
}

bool IsMountedValue::Calculate()
{
    Unit* target = GetTarget();
    if (!target)
        return false;

    return target->IsMounted();
}


bool IsInCombatValue::Calculate()
{
    Unit* target = GetTarget();
    if (!target)
        return false;

    if (sServerFacade.IsInCombat(target)) return true;

    if (target == bot)
    {
        Group* group = bot->GetGroup();
        if (group)
        {
            Group::MemberSlotList const& groupSlot = group->GetMemberSlots();
            for (Group::member_citerator itr = groupSlot.begin(); itr != groupSlot.end(); itr++)
            {
                Player *member = sObjectMgr.GetPlayer(itr->guid);
                if (!member || member == bot) continue;

                if (sServerFacade.IsInCombat(member) &&
                    sServerFacade.IsDistanceLessOrEqualThan(sServerFacade.GetDistance2d(member, bot), sPlayerbotAIConfig.reactDistance)) return true;
            }
        }
    }

    return false;
}

uint8 BagSpaceValue::Calculate()
{
    uint32 totalused = 0, total = 16;
    for (uint8 slot = INVENTORY_SLOT_ITEM_START; slot < INVENTORY_SLOT_ITEM_END; slot++)
    {
        if (bot->GetItemByPos(INVENTORY_SLOT_BAG_0, slot))
            totalused++;
    }

    uint32 totalfree = 16 - totalused;
    for (uint8 bag = INVENTORY_SLOT_BAG_START; bag < INVENTORY_SLOT_BAG_END; ++bag)
    {
        const Bag* const pBag = (Bag*) bot->GetItemByPos(INVENTORY_SLOT_BAG_0, bag);
        if (pBag)
        {
            ItemPrototype const* pBagProto = pBag->GetProto();
            if (pBagProto->Class == ITEM_CLASS_CONTAINER && pBagProto->SubClass == ITEM_SUBCLASS_CONTAINER)
            {
                total += pBag->GetBagSize();
                totalfree += pBag->GetFreeSlots();
                totalused += pBag->GetBagSize() - pBag->GetFreeSlots();
            }
        }

    }

    return (static_cast<float> (totalused) / total) * 100;
}

uint8 DurabilityValue::Calculate()
{
    uint32 totalMax = 0, total = 0;

    for (int i = EQUIPMENT_SLOT_START; i < INVENTORY_SLOT_ITEM_END; ++i)
    {
        uint16 pos = ((INVENTORY_SLOT_BAG_0 << 8) | i);
        Item* item = bot->GetItemByPos(pos);

        if (!item)
            continue;

        uint32 maxDurability = item->GetUInt32Value(ITEM_FIELD_MAXDURABILITY);
        if (!maxDurability)
            continue;

        totalMax += maxDurability;

        uint32 curDurability = item->GetUInt32Value(ITEM_FIELD_DURABILITY);

        total += curDurability;
    }

    if (total == 0)
        return 0;

    return (static_cast<float> (total) / totalMax) * 100;
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

uint8 SpeedValue::Calculate()
{
    Unit* target = GetTarget();
    if (!target)
        return 100;

    return (uint8) (100.0f * target->GetSpeedRate(MOVE_RUN));
}

