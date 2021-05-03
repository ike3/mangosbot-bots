#pragma once
#include "WorldBuffAction.h"


using namespace ai;

bool WorldBuffAction::Execute(Event event)
{    
    string text = event.getParam();

    for (auto& wb : NeedWorldBuffs(bot))
    {
        AddAura(bot, wb);
    }

    return false;
}

vector<uint32> WorldBuffAction::NeedWorldBuffs(Unit* unit)
{
    vector<uint32> retVec;

    if (sPlayerbotAIConfig.worldBuffs.empty())
        return retVec;
   
    FactionTemplateEntry const* humanFaction = sFactionTemplateStore.LookupEntry(1);
    uint32 factionId = PlayerbotAI::GetFactionReaction(unit->GetFactionTemplateEntry(), humanFaction) >= REP_NEUTRAL ? 1 : 2;

    for (auto& wb : sPlayerbotAIConfig.worldBuffs)
    {
        if (wb.factionId != 0 && wb.factionId != factionId)
            continue;

        if (wb.classId != 0 && wb.classId != unit->getClass())
            continue;

        if (wb.minLevel != 0 && wb.minLevel > unit->getLevel())
            continue;

        if (wb.maxLevel != 0 && wb.maxLevel < unit->getLevel())
            continue;

        if (unit->HasAura(wb.spellId))
            continue;

        retVec.push_back(wb.spellId);
    }

    return retVec;
}

bool WorldBuffAction::AddAura(Unit* unit, uint32 spellId)
{
    // number or [name] Shift-click form |color|Hspell:spell_id|h[name]|h|r or Htalent form    

    SpellEntry const* spellInfo = sSpellTemplate.LookupEntry<SpellEntry>(spellId);
    if (!spellInfo)
        return false;

    if (!IsSpellAppliesAura(spellInfo, (1 << EFFECT_INDEX_0) | (1 << EFFECT_INDEX_1) | (1 << EFFECT_INDEX_2)) &&
        !IsSpellHaveEffect(spellInfo, SPELL_EFFECT_PERSISTENT_AREA_AURA))
    {
        return false;
    }

    SpellAuraHolder* holder = CreateSpellAuraHolder(spellInfo, unit, unit);

    for (uint32 i = 0; i < MAX_EFFECT_INDEX; ++i)
    {
        uint8 eff = spellInfo->Effect[i];
        if (eff >= MAX_SPELL_EFFECTS)
            continue;
        if (IsAreaAuraEffect(eff) ||
            eff == SPELL_EFFECT_APPLY_AURA ||
            eff == SPELL_EFFECT_PERSISTENT_AREA_AURA)
        {
            int32 basePoints = spellInfo->CalculateSimpleValue(SpellEffectIndex(i));
            int32 damage = 0; // no damage cos caster doesnt exist
            Aura* aur = CreateAura(spellInfo, SpellEffectIndex(i), &damage, &basePoints, holder, unit);
            holder->AddAura(aur, SpellEffectIndex(i));
        }
    }
    if (!unit->AddSpellAuraHolder(holder))
        delete holder;

    return true;
}