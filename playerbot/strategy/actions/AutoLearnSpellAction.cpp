#include "botpch.h"
#include "../../playerbot.h"
#include "AutoLearnSpellAction.h"
#include "ServerFacade.h"


using namespace ai;

bool AutoLearnSpellAction::Execute(Event event)
{
    string param = event.getParam();

    ostringstream out;

    LearnSpells(&out);

    if (!out.str().empty())
    {
        const std::string& temp = out.str();
        out.seekp(0);
        out << "Learned spells: ";
        out << temp;
        out.seekp(-2, out.cur);
        out << ".";
        ai->TellMaster(out);
    }

    return true;
}


void AutoLearnSpellAction::LearnSpells(ostringstream* out)
{
    if (sPlayerbotAIConfig.autoLearnTrainerSpells)// || (!ai->GetMaster() && sRandomPlayerbotMgr.IsRandomBot(bot)))    
        LearnTrainerSpells(out);

    if (sPlayerbotAIConfig.autoLearnQuestSpells)// || (!ai->GetMaster() && sRandomPlayerbotMgr.IsRandomBot(bot)))
        LearnQuestSpells(out);
}

void AutoLearnSpellAction::LearnTrainerSpells(ostringstream* out)
{
    bot->learnDefaultSpells();

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
            {
                SpellEntry const* spell = sServerFacade.LookupSpellInfo(tSpell->spell);
                if (spell)
                {
                    string SpellName = spell->SpellName[0];                    
                    if (spell->Effect[EFFECT_INDEX_1] == SPELL_EFFECT_SKILL_STEP)
                    {
                        uint32 skill = spell->EffectMiscValue[EFFECT_INDEX_1];

                        if (skill)
                        {                            
                            SkillLineEntry const* pSkill = sSkillLineStore.LookupEntry(skill);
                            if (pSkill)
                            {
                                if (SpellName.find("Apprentice") != string::npos && pSkill->categoryId == SKILL_CATEGORY_PROFESSION || pSkill->categoryId == SKILL_CATEGORY_SECONDARY)
                                    continue;                                
                            }
                        }
                    }
                }

            }

            LearnSpell(tSpell->spell, out);
        }
    }
}

void AutoLearnSpellAction::LearnQuestSpells(ostringstream* out)
{
    //CreatureInfo const* co = sCreatureStorage.LookupEntry<CreatureInfo>(id);
    ObjectMgr::QuestMap const& questTemplates = sObjectMgr.GetQuestTemplates();
    for (ObjectMgr::QuestMap::const_iterator i = questTemplates.begin(); i != questTemplates.end(); ++i)
    {
        uint32 questId = i->first;
        Quest const* quest = i->second;

        if (!quest->GetRequiredClasses() || quest->IsRepeatable() || quest->GetMinLevel() < 10)
            continue;

        if (!bot->SatisfyQuestClass(quest, false) ||
            quest->GetMinLevel() > bot->getLevel() ||
            !bot->SatisfyQuestRace(quest, false))
            continue;

        if (quest->GetRewSpellCast() > 0)
        {
            LearnSpell(quest->GetRewSpellCast(), out);
        }
        else if (quest->GetRewSpell() > 0)
        {
            LearnSpell(quest->GetRewSpell(), out);
        }
    }
}

string formatSpell(SpellEntry const* sInfo)
{
    ostringstream out;
    string rank = sInfo->Rank[0];
    
    if(rank.empty())
        out << "|cffffffff|Hspell:" << sInfo->Id << "|h[" << sInfo->SpellName[LOCALE_enUS] << "]|h|r";
    else
        out << "|cffffffff|Hspell:" << sInfo->Id << "|h[" << sInfo->SpellName[LOCALE_enUS] << " " << rank << "]|h|r";
    return out.str();
}

void AutoLearnSpellAction::LearnSpell(uint32 spellId, ostringstream* out)
{
    SpellEntry const* proto = sServerFacade.LookupSpellInfo(spellId);

    if (!proto)
        return;

#ifdef CMANGOS
    bool learned = false;
    for (int j = 0; j < 3; ++j)
    {
        if (proto->Effect[j] == SPELL_EFFECT_LEARN_SPELL)
        {
            uint32 learnedSpell = proto->EffectTriggerSpell[j];

            if (!bot->HasSpell(learnedSpell))
            {
                bot->learnSpell(learnedSpell, false);
                SpellEntry const* spellInfo = sServerFacade.LookupSpellInfo(learnedSpell);
                *out << formatSpell(spellInfo) << ", ";
            }
            learned = true;
        }
    }
    if (!learned) {
        ai->CastSpell(spellId, bot);
        *out << formatSpell(proto) << ", ";
    }
#endif

#ifdef MANGOS
    bool learned = false;
    for (int j = 0; j < 3; ++j)
    {
        if (proto->Effect[j] == SPELL_EFFECT_LEARN_SPELL)
        {
            uint32 learnedSpell = proto->EffectTriggerSpell[j];

            if (!bot->HasSpell(learnedSpell))
            {
                bot->learnSpell(learnedSpell, false);
                SpellEntry const* spellInfo = sServerFacade.LookupSpellInfo(learnedSpell);
                *out << formatSpell(spellInfo) << ", ";
            }
            learned = true;
        }
    }
    if (!learned) {
        if (!bot->HasSpell(spellId))
        {
            bot->learnSpell(spellId, false);
            *out << formatSpell(proto) << ", ";
        }
    }
#endif
}