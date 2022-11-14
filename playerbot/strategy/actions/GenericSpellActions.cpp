#include "botpch.h"
#include "../../playerbot.h"
#include "GenericActions.h"

using namespace ai;

bool CastSpellAction::Execute(Event& event)
{
    if (spellName == "conjure food" || spellName == "conjure water")
    {
        uint32 castId = 0;

        for (PlayerSpellMap::iterator itr = bot->GetSpellMap().begin(); itr != bot->GetSpellMap().end(); ++itr)
        {
            uint32 spellId = itr->first;

            const SpellEntry* pSpellInfo = sServerFacade.LookupSpellInfo(spellId);
            if (!pSpellInfo)
                continue;

            string namepart = pSpellInfo->SpellName[0];
            strToLower(namepart);

            if (namepart.find(spellName) == string::npos)
                continue;

            if (pSpellInfo->Effect[0] != SPELL_EFFECT_CREATE_ITEM)
                continue;

            uint32 itemId = pSpellInfo->EffectItemType[0];
            ItemPrototype const *proto = sObjectMgr.GetItemPrototype(itemId);
            if (!proto)
                continue;

            if (bot->CanUseItem(proto) != EQUIP_ERR_OK)
                continue;

            if (pSpellInfo->Id > castId)
                castId = pSpellInfo->Id;
        }

        return ai->CastSpell(castId, bot);
    }

	return ai->CastSpell(spellName, GetTarget());
}

bool CastSpellAction::isPossible()
{
    if (ai->IsInVehicle() && !ai->IsInVehicle(false, false, true))
    {
        return false;
    }

    if (spellName == "mount")
    {
        if (!bot->IsMounted() && !bot->IsInCombat())
        {
            return true;
        }
        if (bot->IsInCombat())
        {
            bot->Unmount();
            return false;
        }
    }

    // Check if the ignore range flag gas been set
    bool ignoreRange = !qualifier.empty() ? Qualified::getMultiQualifierInt(qualifier, 1, ":") : false;
	return ai->CanCastSpell(spellName, GetTarget(), true, nullptr, ignoreRange);
}

bool CastSpellAction::isUseful()
{
    if (ai->IsInVehicle() && !ai->IsInVehicle(false, false, true))
        return false;

    if (spellName == "mount" && !bot->IsMounted() && !bot->IsInCombat())
        return true;
    if (spellName == "mount" && bot->IsInCombat())
    {
        bot->Unmount();
        return false;
    }

    bool isUsefulCast = AI_VALUE2(bool, "spell cast useful", spellName);

    Unit* spellTarget = GetTarget();
    if (!spellTarget)
        return false;

    if (!spellTarget->IsInWorld() || spellTarget->GetMapId() != bot->GetMapId())
        return false;

    bool canReach = false;

    if (spellTarget == bot)
        canReach = true;
    else
    {
        float dist = bot->GetDistance(spellTarget, true, ai->IsRanged(bot) ? DIST_CALC_COMBAT_REACH : DIST_CALC_COMBAT_REACH_WITH_MELEE);
        if (range == ATTACK_DISTANCE) // melee action
        {
            canReach = bot->CanReachWithMeleeAttack(spellTarget);
        }
        else // range check
        {
            canReach = dist <= (range + sPlayerbotAIConfig.contactDistance);
            if (!spellId)
                return true; // there can be known alternatives

            const SpellEntry* pSpellInfo = sServerFacade.LookupSpellInfo(spellId);
            if (!pSpellInfo)
                return true; // there can be known alternatives

            if (range != ATTACK_DISTANCE && pSpellInfo->rangeIndex != SPELL_RANGE_IDX_COMBAT && pSpellInfo->rangeIndex != SPELL_RANGE_IDX_SELF_ONLY && pSpellInfo->rangeIndex != SPELL_RANGE_IDX_ANYWHERE)
            {
                SpellRangeEntry const* srange = sSpellRangeStore.LookupEntry(pSpellInfo->rangeIndex);
                float max_range = GetSpellMaxRange(srange);
                float min_range = GetSpellMinRange(srange);
                canReach = dist < max_range && dist >= min_range;
            }
        }
    }

    return spellTarget && isUsefulCast && canReach; // bot->GetDistance(spellTarget, true, DIST_CALC_COMBAT_REACH) <= (range + sPlayerbotAIConfig.contactDistance);
}

void CastSpellAction::SetSpellName(const string& name, string spellIDContextName /*= "spell id"*/)
{
    if (spellName != name)
    {
        spellName = name;
        spellId = ai->GetAiObjectContext()->GetValue<uint32>(spellIDContextName, name)->Get();
    }
}

string CastSpellAction::GetTargetName()
{
    string targetName = "current target";

    // Check if the target name has been overridden
    if (!qualifier.empty())
    {
        targetName = Qualified::getMultiQualifierStr(qualifier, 0, ":");
    }

    return targetName;
}

bool CastAuraSpellAction::isUseful()
{
    return GetTarget() && (GetTarget() != nullptr) && (GetTarget() != NULL) && CastSpellAction::isUseful() && !ai->HasAura(GetSpellName(), GetTarget(), false, isOwner);
}

bool CastEnchantItemAction::isPossible()
{
    if (!CastSpellAction::isPossible())
        return false;

    return GetSpellID() && AI_VALUE2(Item*, "item for spell", GetSpellID());
}

bool CastAoeHealSpellAction::isUseful()
{
    return CastSpellAction::isUseful();
}

Value<Unit*>* CurePartyMemberAction::GetTargetValue()
{
    return context->GetValue<Unit*>("party member to dispel", dispelType);
}

Value<Unit*>* BuffOnPartyAction::GetTargetValue()
{
    return context->GetValue<Unit*>("party member without aura", GetSpellName());
}

bool CastVehicleSpellAction::isPossible()
{
    return ai->CanCastVehicleSpell(GetSpellID(), GetTarget());
}

bool CastVehicleSpellAction::isUseful()
{
    return ai->IsInVehicle(false, true);
}

bool CastVehicleSpellAction::Execute(Event& event)
{
    return ai->CastVehicleSpell(GetSpellID(), GetTarget());
}

bool CastShootAction::isPossible()
{
    // Check if the bot has a ranged weapon equipped and has ammo
    UpdateWeaponInfo();
    if (rangedWeapon && !needsAmmo)
    {
        // Check if the target exist and it can be shot
        Unit* target = GetTarget();
        if (target && sServerFacade.IsWithinLOSInMap(bot, target))
        {
            return CastSpellAction::isPossible();
        }
    }

    return false;
}

bool CastShootAction::Execute(Event& event)
{
    bool succeeded = false;

    UpdateWeaponInfo();
    if (rangedWeapon && !needsAmmo)
    {
        // Prevent calling the shoot spell when already active
        Spell* autoRepeatSpell = ai->GetBot()->GetCurrentSpell(CURRENT_AUTOREPEAT_SPELL);
        if (autoRepeatSpell && (autoRepeatSpell->m_spellInfo->Id == GetSpellID()))
        {
            succeeded = true;
        }
        else if (CastSpellAction::Execute(event))
        {
            succeeded = true;
        }

        if (succeeded)
        {
            SetDuration(weaponDelay);
        }
    }

    return succeeded;
}

void CastShootAction::UpdateWeaponInfo()
{
    // Check if we have a new ranged weapon equipped
    const Item* equippedWeapon = bot->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_RANGED);
    if (equippedWeapon)
    {
        if (equippedWeapon != rangedWeapon)
        {
            string spellName = "shoot";
            bool isRangedWeapon = false;

            const ItemPrototype* itemPrototype = equippedWeapon->GetProto();
            switch (itemPrototype->SubClass)
            {
                case ITEM_SUBCLASS_WEAPON_GUN:
                {
                    isRangedWeapon = true;
                    spellName += " gun";
                    break;
                }
                case ITEM_SUBCLASS_WEAPON_BOW:
                {
                    isRangedWeapon = true;
                    spellName += " bow";
                    break;
                }
                case ITEM_SUBCLASS_WEAPON_CROSSBOW:
                {
                    isRangedWeapon = true;
                    spellName += " crossbow";
                    break;
                }

                case ITEM_SUBCLASS_WEAPON_WAND:
                {
                    isRangedWeapon = true;
                    break;
                }

                default: break;
            }

            // Set the new weapon parameters
            if (isRangedWeapon)
            {
                SetSpellName(spellName);
                rangedWeapon = equippedWeapon;
                weaponDelay = itemPrototype->Delay + sPlayerbotAIConfig.globalCoolDown;
            }
        }

        // Check the ammunition
        needsAmmo = (GetSpellName() != "shoot") ? (AI_VALUE2(uint32, "item count", "ammo") <= 0) : false;
    }
    else
    {
        rangedWeapon = nullptr;
    }
}
