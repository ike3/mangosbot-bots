#pragma once
#include "CraftValues.h"
#include "ItemUsageValue.h"
#include "../../ServerFacade.h"

vector<uint32> CraftSpellsValue::Calculate()
{
    vector<uint32> spellIds;

    PlayerSpellMap const& spellMap = bot->GetSpellMap();

    for (auto& spell : spellMap)
    {
        uint32 spellId = spell.first;

        if (spell.second.state == PLAYERSPELL_REMOVED || spell.second.disabled || IsPassiveSpell(spellId))
            continue;

        const SpellEntry* pSpellInfo = sServerFacade.LookupSpellInfo(spellId);
        if (!pSpellInfo)
            continue;

        if (pSpellInfo->Effect[0] != SPELL_EFFECT_CREATE_ITEM)
            continue;      

        for (uint8 i = 0; i < MAX_EFFECT_INDEX; i++)
            if (pSpellInfo->EffectItemType[i])
            {
                spellIds.push_back(spellId);
                break;
            }
    }

    return spellIds;
}

bool HasReagentsForValue::Calculate()
{
    if (ai->HasCheat(BotCheatMask::item))
        return true;

    uint32 spellId = stoi(getQualifier());

    const SpellEntry* pSpellInfo = sServerFacade.LookupSpellInfo(spellId);

    if (!pSpellInfo)
        return false;

    for (uint8 i = 0; i < MAX_SPELL_REAGENTS; i++)
        if (pSpellInfo->ReagentCount[i] > 0 && pSpellInfo->Reagent[i])
        {
            const ItemPrototype* reqProto = sObjectMgr.GetItemPrototype(pSpellInfo->Reagent[i]);

            uint32 count = AI_VALUE2(uint32, "item count", reqProto->Name1);

            if (count < pSpellInfo->ReagentCount[i])
                return false;
        }

    return true;
}

bool CanCraftSpellValue::Calculate()
{
    uint32 spellId = stoi(getQualifier());

    const SpellEntry* pSpellInfo = sServerFacade.LookupSpellInfo(spellId);

    if (!pSpellInfo)
        return false;

    if (!AI_VALUE2(bool, "has reagents for", spellId))
        return false;

    return true;
}

bool ShouldCraftSpellValue::Calculate()
{
    uint32 spellId = stoi(getQualifier());

    if (SpellGivesSkillUp(spellId, bot))
        return true;

    const SpellEntry* pSpellInfo = sServerFacade.LookupSpellInfo(spellId);
    if (!pSpellInfo)
        return false;

    for (uint8 i = 0; i < MAX_EFFECT_INDEX; i++)
        if (pSpellInfo->EffectItemType[i])
        {
            ItemUsage usage = AI_VALUE2(ItemUsage, "item usage", to_string(pSpellInfo->EffectItemType[i]));

            bool needItem = false;

            switch (usage)
            {
            case ITEM_USAGE_EQUIP:
            case ITEM_USAGE_REPLACE:
            case ITEM_USAGE_BAD_EQUIP:
            case ITEM_USAGE_QUEST:
            case ITEM_USAGE_USE:
                needItem = true;
                break;
            case ITEM_USAGE_FORCE:
            {
                ForceItemUsage forceUsage = AI_VALUE2(ForceItemUsage, "force item usage", to_string(pSpellInfo->EffectItemType[i]));

                needItem = forceUsage == ForceItemUsage::FORCE_USAGE_NEED;
                break;
            }
            case ITEM_USAGE_SKILL:
            case ITEM_USAGE_AMMO:
            case ITEM_USAGE_DISENCHANT:
            case ITEM_USAGE_AH:
            case ITEM_USAGE_VENDOR:
                needItem = !ai->HasCheat(BotCheatMask::item);
                break;
            case ITEM_USAGE_NONE:
            case ITEM_USAGE_KEEP:
            case ITEM_USAGE_BROKEN_EQUIP:
            case ITEM_USAGE_GUILD_TASK:

                needItem = false;
            default:
                needItem = false;
            }

            if (needItem)
                return true;
        }

    return false;
}




inline int SkillGainChance(uint32 SkillValue, uint32 GrayLevel, uint32 GreenLevel, uint32 YellowLevel)
{
    if (SkillValue >= GrayLevel)
        return sWorld.getConfig(CONFIG_UINT32_SKILL_CHANCE_GREY) * 10;
    if (SkillValue >= GreenLevel)
        return sWorld.getConfig(CONFIG_UINT32_SKILL_CHANCE_GREEN) * 10;
    if (SkillValue >= YellowLevel)
        return sWorld.getConfig(CONFIG_UINT32_SKILL_CHANCE_YELLOW) * 10;
    return sWorld.getConfig(CONFIG_UINT32_SKILL_CHANCE_ORANGE) * 10;
}

bool ShouldCraftSpellValue::SpellGivesSkillUp(uint32 spellId, Player* bot)
{
    SkillLineAbilityMapBounds bounds = sSpellMgr.GetSkillLineAbilityMapBoundsBySpellId(spellId);

    for (SkillLineAbilityMap::const_iterator _spell_idx = bounds.first; _spell_idx != bounds.second; ++_spell_idx)
    {
        SkillLineAbilityEntry const* skill = _spell_idx->second;
        if (skill->skillId)
        {
            uint32 SkillValue = bot->GetSkillValuePure(skill->skillId);

            uint32 craft_skill_gain = sWorld.getConfig(CONFIG_UINT32_SKILL_GAIN_CRAFTING);

            if (SkillGainChance(SkillValue,
                skill->max_value,
                (skill->max_value + skill->min_value) / 2,
                skill->min_value) > 0)
                return true;
        }
    }

    return false;
}