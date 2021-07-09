#include "botpch.h"
#include "../../playerbot.h"
#include "CheckMountStateAction.h"

#include "../../ServerFacade.h"
#include "BattleGroundWS.h"
using namespace ai;

uint64 extractGuid(WorldPacket& packet);

bool CheckMountStateAction::Execute(Event event)
{
    bool noattackers = AI_VALUE(uint8, "attacker count") > 0 ? false : true;
    bool enemy = AI_VALUE(Unit*, "enemy player target");
    bool dps = (AI_VALUE(Unit*, "dps target") || AI_VALUE(Unit*, "grind target"));
    bool fartarget = (enemy && sServerFacade.IsDistanceGreaterThan(AI_VALUE2(float, "distance", "enemy player target"), 40.0f)) ||
        (dps && sServerFacade.IsDistanceGreaterThan(AI_VALUE2(float, "distance", "dps target"), 50.0f));
    bool attackdistance = false;
    bool chasedistance = false;
    float attack_distance = 35.0f;

    switch (bot->getClass())
    {
    case CLASS_WARRIOR:
    case CLASS_PALADIN:
        attack_distance = 10.0f;
        break;
    case CLASS_ROGUE:
        attack_distance = 40.0f;
        break;
    }
    if (enemy)
        attack_distance /= 2;

    if (dps || enemy)
    {
        attackdistance = sServerFacade.IsDistanceLessThan(AI_VALUE2(float, "distance", "current target"), attack_distance);
        chasedistance = enemy && sServerFacade.IsDistanceGreaterThan(AI_VALUE2(float, "distance", "enemy player target"), 45.0f) && AI_VALUE2(bool, "moving", "enemy player target");
    }

    Player* master = GetMaster();
    if (master != nullptr && !bot->InBattleGround())
    {
        if (!bot->GetGroup() || bot->GetGroup()->GetLeaderGuid() != master->GetObjectGuid())
            return false;

        bool farFromMaster = sServerFacade.GetDistance2d(bot, master) > sPlayerbotAIConfig.sightDistance;
        if (master->IsMounted() && !bot->IsMounted() && noattackers)
        {
            return Mount();
        }

        if (!bot->IsMounted() && chasedistance && !bot->IsInCombat() && !dps)
            return Mount();

        if ((farFromMaster || !master->IsMounted() || attackdistance) && bot->IsMounted())
        {
            WorldPacket emptyPacket;
            bot->GetSession()->HandleCancelMountAuraOpcode(emptyPacket);
            return true;
        }
        return false;
    }

    if (bot->InBattleGround() && (noattackers || fartarget) && !bot->IsInCombat() && !bot->IsMounted())
    {
        if (bot->GetBattleGroundTypeId() == BattleGroundTypeId::BATTLEGROUND_WS)
        {
            BattleGroundWS *bg = (BattleGroundWS*)ai->GetBot()->GetBattleGround();

            if (bot->HasAura(23333) || bot->HasAura(23335))
            {
                return false;
            }
#ifdef MANGOSBOT_ZERO
            //check near A Flag
            uint32 lowguid = 90000;
            uint32 id = 179830;
            GameObject* AllianceflagStand = bg->GetBgMap()->GetGameObject(ObjectGuid(HIGHGUID_GAMEOBJECT, id, lowguid));

            if (bot->IsWithinDistInMap(AllianceflagStand, 3.0f))
                return false;

            //check near H Flag
            lowguid = 90001;
            id = 179831;
            GameObject* HordeflagStand = bg->GetBgMap()->GetGameObject(ObjectGuid(HIGHGUID_GAMEOBJECT, id, lowguid));

            if (bot->IsWithinDistInMap(HordeflagStand, 3.0f))
                return false;
#endif
            return Mount();
        }
        return Mount();
    }

    ObjectGuid unit = AI_VALUE(ObjectGuid, "rpg target");
    if (unit)
    {
        if (sServerFacade.IsDistanceGreaterThan(AI_VALUE2(float, "distance", "rpg target"), sPlayerbotAIConfig.farDistance) && noattackers && !dps && !enemy)
            return Mount();
    }

    if (((!AI_VALUE(list<ObjectGuid>, "possible rpg targets").empty()) && noattackers && !dps && !enemy) && urand(0, 100) > 50)
        return Mount();

    if (!bot->IsMounted() && (fartarget || chasedistance))
        return Mount();

    if (attackdistance && bot->IsMounted() && (!noattackers && sServerFacade.IsInCombat(bot)))
    {
        WorldPacket emptyPacket;
        bot->GetSession()->HandleCancelMountAuraOpcode(emptyPacket);
        return true;
    }

    return false;
}

bool CheckMountStateAction::isUseful()
{
    if (bot->IsDead())
        return false;

    bool isOutdoor = bot->GetMap()->GetTerrain()->IsOutdoors(bot->GetPositionX(), bot->GetPositionY(), bot->GetPositionZ());
    if (!isOutdoor)
        return false;

    if (bot->IsTaxiFlying() || bot->IsFlying())
        return false;

#ifndef MANGOSBOT_ZERO
    if (bot->InArena())
        return false;
#endif

    if (!bot->GetPlayerbotAI()->HasStrategy("mount", BOT_STATE_NON_COMBAT) && !bot->IsMounted())
        return false;

    bool firstmount = bot->getLevel() >=
#ifdef MANGOSBOT_ZERO
        40
#else
#ifdef MANGOSBOT_ONE
        30
#else
        20
#endif
#endif
        ;
    if (!firstmount)
        return false;

    // Only mount if BG starts in less than 30 sec
    if (bot->InBattleGround())
    {
        BattleGround *bg = bot->GetBattleGround();
        if (bg && bg->GetStatus() == STATUS_WAIT_JOIN)
        {
            if (bg->GetStartDelayTime() > BG_START_DELAY_30S)
                return false;
        }
    }

    return true;
}

bool CheckMountStateAction::Mount()
{
    if (sServerFacade.isMoving(bot))
    {
        bot->StopMoving();
        bot->GetMotionMaster()->Clear();
        bot->GetMotionMaster()->MoveIdle();
    }

    Player* master = GetMaster();
    ai->RemoveShapeshift();

    int32 masterSpeed = 150;
    const SpellEntry *masterSpell = NULL;

    if (master != NULL && master->GetAurasByType(SPELL_AURA_MOUNTED).size() > 0 && !bot->InBattleGround())
    {
        Unit::AuraList const& auras = master->GetAurasByType(SPELL_AURA_MOUNTED);
        if (auras.empty()) return false;

        masterSpell = auras.front()->GetSpellProto();
        masterSpeed = max(masterSpell->EffectBasePoints[1], masterSpell->EffectBasePoints[2]);
    }
    else
    {
        masterSpeed = 0;
        for (PlayerSpellMap::iterator itr = bot->GetSpellMap().begin(); itr != bot->GetSpellMap().end(); ++itr)
        {
            uint32 spellId = itr->first;
            const SpellEntry* spellInfo = sServerFacade.LookupSpellInfo(spellId);
            if (!spellInfo || spellInfo->EffectApplyAuraName[0] != SPELL_AURA_MOUNTED)
                continue;
            if (itr->second.state == PLAYERSPELL_REMOVED || itr->second.disabled || IsPassiveSpell(spellId))
                continue;
            int32 effect = max(spellInfo->EffectBasePoints[1], spellInfo->EffectBasePoints[2]);
            if (effect > masterSpeed)
                masterSpeed = effect;
        }
    }

    //map<int32, vector<uint32> > spells;
    map<uint32, map<int32, vector<uint32> > > allSpells;
#ifdef MANGOS
    if (bot->GetPureSkillValue(SKILL_RIDING) <= 75 && bot->getLevel() < 60)
#endif
#ifdef CMANGOS
    if (bot->GetSkillValuePure(SKILL_RIDING) <= 75 && bot->getLevel() < 60)
#endif
        masterSpeed = 59;

    if (bot->InBattleGround() && masterSpeed > 99)
        masterSpeed = 99;

    for (PlayerSpellMap::iterator itr = bot->GetSpellMap().begin(); itr != bot->GetSpellMap().end(); ++itr)
    {
        uint32 spellId = itr->first;
        if (itr->second.state == PLAYERSPELL_REMOVED || itr->second.disabled || IsPassiveSpell(spellId))
            continue;

        const SpellEntry* spellInfo = sServerFacade.LookupSpellInfo(spellId);
        if (!spellInfo || spellInfo->EffectApplyAuraName[0] != SPELL_AURA_MOUNTED)
            continue;

        int32 effect = max(spellInfo->EffectBasePoints[1], spellInfo->EffectBasePoints[2]);
        if (effect < masterSpeed)
            continue;

        uint32 index = 0;
#ifndef MANGOSBOT_ZERO
        index = (spellInfo->EffectApplyAuraName[1] == SPELL_AURA_MOD_FLIGHT_SPEED_MOUNTED ||
            spellInfo->EffectApplyAuraName[2] == SPELL_AURA_MOD_FLIGHT_SPEED_MOUNTED) ? 1 : 0;
#endif

        allSpells[index][effect].push_back(spellId);
    }


    int masterMountType = 0;
#ifndef MANGOSBOT_ZERO
    if (masterSpell != NULL)
    {
        masterMountType = (masterSpell->EffectApplyAuraName[1] == SPELL_AURA_MOD_FLIGHT_SPEED_MOUNTED ||
            masterSpell->EffectApplyAuraName[2] == SPELL_AURA_MOD_FLIGHT_SPEED_MOUNTED) ? 1 : 0;
    }
#endif

    map<int32, vector<uint32> >& spells = allSpells[masterMountType];

    for (map<int32, vector<uint32> >::iterator i = spells.begin(); i != spells.end(); ++i)
    {
        vector<uint32>& ids = i->second;
        int index = urand(0, ids.size() - 1);
        if (index >= ids.size())
            continue;

        MotionMaster &mm = *bot->GetMotionMaster();
        ai->CastSpell(ids[index], bot);
        return true;
    }

    list<Item*> items = AI_VALUE2(list<Item*>, "inventory items", "mount");
    if (!items.empty()) return UseItemAuto(*items.begin());

    return false;
}
