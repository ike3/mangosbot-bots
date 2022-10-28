#include "botpch.h"
#include "../../playerbot.h"
#include "PaladinActions.h"

using namespace ai;

string GetActualBlessingOfMight(Unit* target, bool greater = false)
{
    switch (target->getClass())
    {
    case CLASS_MAGE:
    case CLASS_PRIEST:
    case CLASS_WARLOCK:
        return greater ? "greater blessing of wisdom" : "blessing of wisdom";
    }
    return greater ? "greater blessing of might" : "blessing of might";
}

string GetActualBlessingOfWisdom(Unit* target, bool greater = false)
{
    switch (target->getClass())
    {
    case CLASS_WARRIOR:
    case CLASS_ROGUE:
        return greater ? "greater blessing of might" : "blessing of might";
    }
    return greater ? "greater blessing of wisdom" : "blessing of wisdom";
}

Unit* CastBlessingOnPartyAction::GetTarget()
{
    vector<std::string> altBlessings;
    vector<std::string> haveBlessings;
    altBlessings.push_back("blessing of might");
    altBlessings.push_back("blessing of wisdom");
    altBlessings.push_back("blessing of kings");
    if (!bot->InBattleGround())
        altBlessings.push_back("blessing of salvation");
    altBlessings.push_back("blessing of light");

    if (AI_VALUE2(uint32, "spell id", m_name))
        haveBlessings.push_back(m_name);
    for (auto blessing : altBlessings)
    {
        if (AI_VALUE2(uint32, "spell id", blessing) && blessing != m_name)
            haveBlessings.push_back(blessing);
    }
    if (haveBlessings.empty())
        return NULL;

    list<ObjectGuid> nearestPlayers;
    /*if (ai->AllowActivity(OUT_OF_PARTY_ACTIVITY))
        nearestPlayers = AI_VALUE(list<ObjectGuid>, "nearest friendly players");*/

    list<ObjectGuid> nearestGroupPlayers;

    Group* group = bot->GetGroup();
    if (group)
    {
        for (GroupReference* ref = group->GetFirstMember(); ref; ref = ref->next())
        {
            if (!ref->getSource() || bot->GetMapId() != ref->getSource()->GetMapId()) continue;

            if (ai->HasMyAura("blessing of freedom", ref->getSource())) continue;

            if (ref->getSource() != bot && sServerFacade.GetDistance2d(bot, ref->getSource()) < 25.0f)
            {
                if (ref->getSubGroup() != bot->GetSubGroup())
                {
                    nearestGroupPlayers.push_back(ref->getSource()->GetObjectGuid());
                }
                else
                {
                    nearestGroupPlayers.push_front(ref->getSource()->GetObjectGuid());
                }
            }
        }
    }

    //nearestGroupPlayers.insert(nearestGroupPlayers.end(), nearestPlayers.begin(), nearestPlayers.end());
    nearestPlayers = nearestGroupPlayers;
    if (nearestPlayers.empty())
        return NULL;

    Unit* trueTarget = nullptr;
    for (auto guid : nearestPlayers)
    {
        Unit* member = ai->GetUnit(guid);
        if (!member)
            continue;

        bool isMelee = member->IsPlayer() ? (!ai->IsRanged((Player*)member) || !member->HasMana()) : true;
        bool isTank = member->IsPlayer() ? (ai->IsTank((Player*)member)) : false;
        bool hasBlessing = false; 
        bool foundMember = false;
        for (auto blessing : haveBlessings)
        {
            if (ai->HasMyAura(blessing, member) || ai->HasMyAura("greater " + blessing, member))
            {
                hasBlessing = true;
                break;
            }
        }
        if (hasBlessing)
            continue;

        for (auto blessing : haveBlessings)
        {
            if (ai->HasAura(blessing, member) || ai->HasAura("greater " + blessing, member))
                continue;

            if (!isMelee && blessing == "blessing of might")
                continue;

            if (!member->HasMana() && blessing == "blessing of wisdom")
                continue;

            if (isTank && blessing == "blessing of salvation")
                continue;

            if (ai->HasMyAura(blessing, member) || ai->HasMyAura("greater " + blessing, member))
                hasBlessing = true;
            else
                foundMember = true;
        }
        if (!hasBlessing && foundMember)
            return member;
    }

    return NULL;
}

bool CastBlessingOnPartyAction::Execute(Event& event)
{
    Unit* target = GetTarget();
    if (!target) return false;

    std::string mainBlessing = m_name;
    std::string mainGreatBlessing = "greater " + m_name;
    bool isMelee = !ai->IsRanged((Player*)target) || !target->HasMana();
    bool isTank = target->IsPlayer() ? (ai->IsTank((Player*)target)) : false;

    uint32 spellId = AI_VALUE2(uint32, "spell id", mainBlessing);
    Unit* mainTarget = AI_VALUE2(Unit*, "party member without my aura", mainBlessing);
    Unit* greatTarget = AI_VALUE2(Unit*, "party member without my aura", mainGreatBlessing);
    //Value<Unit*>* mainTarget = context->GetValue<Unit*>("party member without my aura", mainBlessing);
    //Value<Unit*>* mainGreaterTarget = context->GetValue<Unit*>("party member without my aura", mainGreatBlessing);

    /*if (mainTarget && !mainGreaterTarget)
        return mainTarget;

    if (mainTarget == mainGreaterTarget)
        return mainGreaterTarget;

    if (mainTarget && mainGreaterTarget && !ai->HasMyAura(mainGreatBlessing, mainTarget->Get())*/

    vector<std::string> altBlessings;
    vector<std::string> haveBlessings;
    altBlessings.push_back("blessing of might");
    altBlessings.push_back("blessing of wisdom");
    altBlessings.push_back("blessing of kings");
    if (!bot->InBattleGround())
        altBlessings.push_back("blessing of salvation");
    altBlessings.push_back("blessing of light");

    if (AI_VALUE2(uint32, "spell id", mainBlessing))
        haveBlessings.push_back(mainBlessing);
    for (auto blessing : altBlessings)
    {
        if (AI_VALUE2(uint32, "spell id", blessing) && blessing != mainBlessing)
            haveBlessings.push_back(blessing);
    }
    if (haveBlessings.empty())
        return false;

    for(auto blessing : haveBlessings)
    {
        bool haveBigBless = false;
        uint32 bigBlessing = AI_VALUE2(uint32, "spell id", "greater " + blessing);
        if (bigBlessing)
        {
            haveBigBless = ai->CanCastSpell(bigBlessing, target, 0, true, nullptr);
        }
        if (!isMelee && blessing == "blessing of might")
            continue;

        if (!target->HasMana() && blessing == "blessing of wisdom")
            continue;

        if (isTank && blessing == "blessing of salvation")
            continue;

        if (!(ai->HasAura(blessing, target) || ai->HasAura("greater " + blessing, target)))
            return ai->CastSpell(haveBigBless ? "greater " + blessing : blessing, target);
    }

    return false;
}

bool CastPaladinAuraAction::Execute(Event& event)
{
    std::string mainAura = m_name;
    vector<std::string> altAuras;
    vector<std::string> haveAuras;
    altAuras.push_back("devotion aura");
    altAuras.push_back("retribution aura");
    altAuras.push_back("concentration aura");
    altAuras.push_back("sanctity aura");
    altAuras.push_back("shadow resistance aura");
    altAuras.push_back("fire resistance aura");
    altAuras.push_back("frost resistance aura");
    altAuras.push_back("crusader aura");

    if (AI_VALUE2(uint32, "spell id", mainAura))
        haveAuras.push_back(mainAura);
    for (auto aura : altAuras)
    {
        if (AI_VALUE2(uint32, "spell id", aura) && aura != mainAura)
            haveAuras.push_back(aura);
    }
    if (haveAuras.empty())
        return false;

    for (auto aura : haveAuras)
    {
        if (!ai->HasAura(aura, bot))
            return ai->CastSpell(aura, bot);
    }
    return false;
}

bool CastBlessingOfMightAction::Execute(Event& event)
{
    Unit* target = GetTarget();
    if (!target) return false;

    return ai->CastSpell(GetActualBlessingOfMight(target), target);
}

bool CastGreaterBlessingOfMightAction::Execute(Event& event)
{
    Unit* target = GetTarget();
    if (!target) return false;

    return ai->CastSpell(GetActualBlessingOfMight(target, true), target);
}

bool CastBlessingOfWisdomAction::Execute(Event& event)
{
    Unit* target = GetTarget();
    if (!target) return false;

    return ai->CastSpell(GetActualBlessingOfWisdom(target), target);
}

bool CastGreaterBlessingOfWisdomAction::Execute(Event& event)
{
    Unit* target = GetTarget();
    if (!target) return false;

    return ai->CastSpell(GetActualBlessingOfWisdom(target, true), target);
}
