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
#ifdef MANGOS 
#ifdef MANGOSBOT_ZERO
    PetDatabaseStatus status = Pet::GetStatusFromDB(bot);
    if (status == PET_DB_DEAD)
        return true;
#endif
#endif
#ifdef CMANGOS
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
#endif

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

    for (uint8 bag = INVENTORY_SLOT_BAG_START; bag < INVENTORY_SLOT_BAG_END; ++bag)
    {
        const Bag* const pBag = (Bag*) bot->GetItemByPos(INVENTORY_SLOT_BAG_0, bag);
        if (pBag)
        {
            ItemPrototype const* pBagProto = pBag->GetProto();
            if (pBagProto->Class == ITEM_CLASS_CONTAINER && pBagProto->SubClass == ITEM_SUBCLASS_CONTAINER)
            {
                total += pBag->GetBagSize();
				totalused += pBag->GetBagSize() - pBag->GetFreeSlots();
            }
        }

    }

    return (static_cast<float> (totalused) / total) * 100;
}

uint8 SpeedValue::Calculate()
{
    Unit* target = GetTarget();
    if (!target)
        return 100;

    return (uint8) (100.0f * target->GetSpeedRate(MOVE_RUN));
}

