#include "botpch.h"
#include "../../playerbot.h"
#include "GenericActions.h"

using namespace ai;

bool CastSpellAction::Execute(Event& event)
{
    if (spell == "conjure food" || spell == "conjure water")
    {
        //uint32 id = AI_VALUE2(uint32, "spell id", spell);
        //if (!id)
        //    return false;

        uint32 castId = 0;

        for (PlayerSpellMap::iterator itr = bot->GetSpellMap().begin(); itr != bot->GetSpellMap().end(); ++itr)
        {
            uint32 spellId = itr->first;

            const SpellEntry* pSpellInfo = sServerFacade.LookupSpellInfo(spellId);
            if (!pSpellInfo)
                continue;

            string namepart = pSpellInfo->SpellName[0];
            strToLower(namepart);

            if (namepart.find(spell) == string::npos)
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

	return ai->CastSpell(spell, GetTarget());
}

bool CastSpellAction::isPossible()
{
    if (ai->IsInVehicle() && !ai->IsInVehicle(false, false, true))
        return false;

    if (spell == "mount" && !bot->IsMounted() && !bot->IsInCombat())
        return true;
    if (spell == "mount" && bot->IsInCombat())
    {
        bot->Unmount();
        return false;
    }

    Spell* currentSpell = bot->GetCurrentSpell(CURRENT_GENERIC_SPELL);

	return ai->CanCastSpell(spell, GetTarget(), true);
}

bool CastSpellAction::isUseful()
{
    if (ai->IsInVehicle() && !ai->IsInVehicle(false, false, true))
        return false;

    if (spell == "mount" && !bot->IsMounted() && !bot->IsInCombat())
        return true;
    if (spell == "mount" && bot->IsInCombat())
    {
        bot->Unmount();
        return false;
    }

    bool isUsefulCast = AI_VALUE2(bool, "spell cast useful", spell);

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

            uint32 spellId = AI_VALUE2(uint32, "spell id", spell);
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

bool CastAuraSpellAction::isUseful()
{
    return GetTarget() && (GetTarget() != nullptr) && (GetTarget() != NULL) && CastSpellAction::isUseful() && !ai->HasAura(spell, GetTarget(), false, isOwner);
}

bool CastEnchantItemAction::isPossible()
{
    if (!CastSpellAction::isPossible())
        return false;

    uint32 spellId = AI_VALUE2(uint32, "spell id", spell);
    return spellId && AI_VALUE2(Item*, "item for spell", spellId);
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
    return context->GetValue<Unit*>("party member without aura", spell);
}

bool CastVehicleSpellAction::isPossible()
{
    uint32 spellId = AI_VALUE2(uint32, "vehicle spell id", spell);
    return ai->CanCastVehicleSpell(spellId, GetTarget());
}

bool CastVehicleSpellAction::isUseful()
{
    return ai->IsInVehicle(false, true);
}

bool CastVehicleSpellAction::Execute(Event& event)
{
    uint32 spellId = AI_VALUE2(uint32, "vehicle spell id", spell);
    return ai->CastVehicleSpell(spellId, GetTarget());
}
