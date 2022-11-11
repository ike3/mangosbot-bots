#include "botpch.h"
#include "../../playerbot.h"
#include "CheckMountStateAction.h"

#include "../../ServerFacade.h"
#include "BattleGroundWS.h"
#include "../../TravelMgr.h"

using namespace ai;

uint64 extractGuid(WorldPacket& packet);

bool CheckMountStateAction::Execute(Event& event)
{
    bool noattackers = AI_VALUE2(bool, "combat", "self target") ? (AI_VALUE(uint8, "attacker count") > 0 ? false : true) : true;
    bool enemy = AI_VALUE(Unit*, "enemy player target");
    bool dps = (AI_VALUE(Unit*, "dps target") || AI_VALUE(Unit*, "grind target"));
    if (enemy || dps)
        noattackers = false;
    bool fartarget = (enemy && sServerFacade.IsDistanceGreaterThan(AI_VALUE2(float, "distance", "enemy player target"), 40.0f)) ||
        (dps && sServerFacade.IsDistanceGreaterThan(AI_VALUE2(float, "distance", "dps target"), 50.0f));

    bool canFly = CanFly();
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
        attack_distance = 50.0f;
        break;
    }
    /*if (enemy)
        attack_distance /= 2;*/

    if (ai->IsHeal(bot) || ai->IsRanged(bot))
        attack_distance = 40.0f;

    if (dps || enemy)
    {
        attackdistance = (enemy || dps) && sServerFacade.IsDistanceLessThan(AI_VALUE2(float, "distance", "current target"), attack_distance);
        chasedistance = enemy && sServerFacade.IsDistanceGreaterThan(AI_VALUE2(float, "distance", "enemy player target"), 45.0f) && AI_VALUE2(bool, "moving", "enemy player target");
    }

    Player* master = GetMaster();
    if (master != nullptr && master != bot && !bot->InBattleGround())
    {
        if (!bot->GetGroup() || bot->GetGroup()->GetLeaderGuid() != master->GetObjectGuid())
            return false;

        bool farFromMaster = bot->GetMapId() != master->GetMapId() ||  sServerFacade.GetDistance2d(bot, master) > sPlayerbotAIConfig.sightDistance;
        if (master->IsMounted() && !bot->IsMounted() && noattackers)
        {
            return Mount();
        }

        if (!bot->IsMounted() && (chasedistance || (farFromMaster && ai->HasStrategy("follow", BotState::BOT_STATE_NON_COMBAT))) && !bot->IsInCombat() && (!dps || canFly))
            return Mount();

        if (!bot->IsFlying() && ((!farFromMaster && !master->IsMounted()) || attackdistance) && bot->IsMounted())
        {
            WorldPacket emptyPacket;
            bot->GetSession()->HandleCancelMountAuraOpcode(emptyPacket);
            return true;
        }
        return false;
    }

    if (bot->InBattleGround() && !attackdistance && (noattackers || fartarget) && !bot->IsInCombat() && !bot->IsMounted())
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

    if (!bot->InBattleGround() && !bot->IsMounted() && !ai->IsStateActive(BotState::BOT_STATE_COMBAT))
    {
        GuidPosition unit = AI_VALUE(GuidPosition, "rpg target");
        if (unit)
        {
            if (sServerFacade.IsDistanceGreaterThan(AI_VALUE2(float, "distance", "rpg target"), sPlayerbotAIConfig.sightDistance) && noattackers && !dps && !enemy)
                return Mount();
        }

        if (!AI_VALUE(TravelTarget*, "travel target")->isTraveling() && ((!AI_VALUE(list<ObjectGuid>, "possible rpg targets").empty()) && noattackers && !dps && !enemy) && urand(0, 100) > 50)
            return Mount();

        if (AI_VALUE(TravelTarget*, "travel target")->isTraveling() && AI_VALUE(TravelTarget*, "travel target")->distance(bot) > sPlayerbotAIConfig.farDistance && !enemy && AI_VALUE(bool, "can move around"))
            return Mount();
    }

    if (!bot->IsMounted() && !attackdistance && (fartarget || chasedistance))
        return Mount();

    if ((!bot->IsFlying() || WorldPosition(bot).currentHeight() < 10.0f)  && attackdistance && bot->IsMounted() && (enemy || (dps && !AI_VALUE(TravelTarget*, "travel target")->isTraveling()) || (!noattackers && sServerFacade.IsInCombat(bot))))
    {
        WorldPacket emptyPacket;
        bot->GetSession()->HandleCancelMountAuraOpcode(emptyPacket);
        return true;
    }

    if ((!bot->IsFlying() || WorldPosition(bot).currentHeight() < 10.0f) && bot->IsMounted() && (AI_VALUE(TravelTarget*, "travel target")->isWorking() || sServerFacade.IsDistanceLessThan(AI_VALUE2(float, "distance", "rpg target"), sPlayerbotAIConfig.farDistance)))
    {
        WorldPacket emptyPacket;
        bot->GetSession()->HandleCancelMountAuraOpcode(emptyPacket);
        return true;
    }

    return false;
}

bool CheckMountStateAction::isUseful()
{
    // do not use on vehicle
    if (ai->IsInVehicle())
        return false;

    if (bot->IsDead())
        return false;

    bool isOutdoor = bot->GetMap()->GetTerrain()->IsOutdoors(bot->GetPositionX(), bot->GetPositionY(), bot->GetPositionZ());
    if (!isOutdoor)
        return false;

    if (bot->IsTaxiFlying())
        return false;

    if (bot->getClass() == CLASS_ROGUE && bot->InBattleGround() && (ai->HasAura("stealth", bot) || ai->HasAura("sprint", bot)))
        return false;

    if (bot->getClass() == CLASS_DRUID && bot->InBattleGround() && (ai->HasAura("prowl", bot) || ai->HasAura("dash", bot)))
        return false;

#ifndef MANGOSBOT_ZERO
    if (bot->InArena())
        return false;
#endif

    if (!bot->GetPlayerbotAI()->HasStrategy("mount", BotState::BOT_STATE_NON_COMBAT) && !bot->IsMounted())
        return false;

    bool firstmount = bot->GetLevel() >=
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

    // Do not use with BG Flags
    if (bot->HasAura(23333) || bot->HasAura(23335) || bot->HasAura(34976))
    {
        return false;
    }

    // Only mount if BG starts in less than 30 sec
    if (bot->InBattleGround())
    {
        BattleGround* bg = bot->GetBattleGround();
        if (bg && bg->GetStatus() == STATUS_WAIT_JOIN)
        {
            if (bg->GetStartDelayTime() > BG_START_DELAY_30S)
                return false;
        }
    }

    return true;
}

bool CheckMountStateAction::CanFly()
{
    if (bot->GetMapId() != 530 && bot->GetMapId() != 571)
        return false;

#ifdef MANGOSBOT_ONE
    uint32 zone, area;
    bot->GetZoneAndAreaId(zone, area);
    uint32 v_map = GetVirtualMapForMapAndZone(bot->GetMapId(), zone);
    MapEntry const* mapEntry = sMapStore.LookupEntry(v_map);
    if (!mapEntry || mapEntry->addon < 1 || !mapEntry->IsContinent())
        return false;
#endif
#ifdef MANGOSBOT_TWO
    uint32 zone, area;
    bot->GetZoneAndAreaId(zone, area);
    if (!bot->CanStartFlyInArea(bot->GetMapId(), zone, area, false))
        return false;
#endif

    if (GetBestMountSpells(true).empty() && GetBestMounts(true).empty())
        return false;

    return true;
}

uint32 CheckMountStateAction::MountSpeed(const SpellEntry* const spellInfo, const bool canFly)
{
    bool isMount = false;

#ifdef MANGOSBOT_ZERO
    uint32 effect = SPELL_AURA_MOD_INCREASE_MOUNTED_SPEED;
#else
    uint32 effect = canFly ? SPELL_AURA_MOD_FLIGHT_SPEED_MOUNTED : SPELL_AURA_MOD_INCREASE_MOUNTED_SPEED; //If we can fly only look at flight speed. Normal mounts then don't get any speed.
#endif

    if (!spellInfo)
        return 0;

    for (int i = 0; i < 3; i++)
    {
        if (spellInfo->EffectApplyAuraName[i] == SPELL_AURA_MOUNTED)
        {
            isMount = true;
            break;
        }
    }

#ifndef MANGOSBOT_ZERO
    //This part stops bots from mounting flying mounts when they can't fly. This should be tweaked if bots ever are able to normally ride flying mounts in the old-world.
    if(isMount && !canFly)
    {
        for (int i = 0; i < 3; i++)
        {
            if (spellInfo->EffectApplyAuraName[i] == SPELL_AURA_MOD_FLIGHT_SPEED_MOUNTED)
            {
                return 0;
            }
        }
    }
#endif

    if (isMount)
    {
        for (int i = 0; i < 3; i++)
        {
            if (spellInfo->EffectApplyAuraName[i] == effect)
            {
                return spellInfo->EffectBasePoints[i];
            }
        }
    }

    return 0;
}

vector<uint32> CheckMountStateAction::GetBestMountSpells(const bool canFly)
{
    uint32 bestMountSpeed = 1;
    vector<uint32> spells;
    for (PlayerSpellMap::iterator itr = bot->GetSpellMap().begin(); itr != bot->GetSpellMap().end(); ++itr)
    {
        uint32 spellId = itr->first;
        if (itr->second.state == PLAYERSPELL_REMOVED || itr->second.disabled || IsPassiveSpell(spellId))
            continue;

        const SpellEntry* spellInfo = sServerFacade.LookupSpellInfo(spellId);
        uint32 mountSpeed = MountSpeed(spellInfo, canFly);

        if (mountSpeed < bestMountSpeed)
            continue;

        if (mountSpeed > bestMountSpeed)
            spells.clear();

        spells.push_back(spellId);
    }

    return spells;
}

uint32 CheckMountStateAction::MountSpeed(const ItemPrototype* proto, const bool canFly)
{
    uint32 speed = 0;
    for (int j = 0; j < MAX_ITEM_PROTO_SPELLS; j++)
    {
        const SpellEntry* const spellInfo = sServerFacade.LookupSpellInfo(proto->Spells[j].SpellId);
        speed = MountSpeed(spellInfo, canFly);

        if (speed)
            return speed;
    }

    return 0;
}

vector<Item*> CheckMountStateAction::GetBestMounts(const bool canFly)
{
    list<Item*> items = AI_VALUE2(list<Item*>, "inventory items", "mount");

    uint32 bestMountSpeed = 1;
    vector<Item*> mounts;

    for (auto& item : items)
    {
        uint32 mountSpeed = MountSpeed(item->GetProto(), canFly);

        if (mountSpeed < bestMountSpeed)
            continue;

        if (mountSpeed > bestMountSpeed)
            mounts.clear();

        mounts.push_back(item);
    }

    return mounts;
}

bool CheckMountStateAction::MountWithBestMount(const bool canFly)
{
    vector<uint32> mountSpells = GetBestMountSpells(canFly);
    vector<Item*> mounts = GetBestMounts(canFly);

    if (mountSpells.empty() && mounts.empty())
        return false;

    if (sServerFacade.isMoving(bot))
    {
        ai->StopMoving();
        return true;
    }

    if (mounts.empty() || (!mountSpells.empty() && MountSpeed(sServerFacade.LookupSpellInfo(mountSpells.front()), canFly) > MountSpeed(mounts.front()->GetProto(), canFly)))
    {
        uint32 spellId = mountSpells[urand(0, mountSpells.size() - 1)];
        return ai->CastSpell(spellId, bot);
    }
    else
    {
        Item* mount = mounts[urand(0, mounts.size() - 1)];
        if (UseItemAuto(mount))
        {
            SetDuration(3000U); // 3s
            return true;
        }
    }

    return false;
}

bool CheckMountStateAction::Mount()
{
    uint32 secondmount =
#ifdef MANGOSBOT_ZERO
        60
#else
#ifdef MANGOSBOT_ONE
        60
#else
        40
#endif
#endif
        ;

    uint32 thirdmount =
#ifdef MANGOSBOT_ZERO
        90
#else
#ifdef MANGOSBOT_ONE
        68
#else
        60
#endif
#endif
        ;

    uint32 fourthmount =
#ifdef MANGOSBOT_ZERO
        90
#else
#ifdef MANGOSBOT_ONE
        70
#else
        70
#endif
#endif
        ;

    Player* master = GetMaster();
    ai->RemoveShapeshift();

    bool didMount = false;

    if (CanFly())
    {
       didMount = MountWithBestMount(true);
    }

    if (!didMount)
        didMount = MountWithBestMount();

    return didMount;


    //BELOW HERE IS OLD STUFF

    int32 masterSpeed = 59;
    const SpellEntry* masterSpell = NULL;

    if (master && master->GetAurasByType(SPELL_AURA_MOUNTED).size() > 0 && !bot->InBattleGround())
    {
        Unit::AuraList const& auras = master->GetAurasByType(SPELL_AURA_MOUNTED);
        if (auras.empty()) return false;

        masterSpell = auras.front()->GetSpellProto();
        masterSpeed = max(masterSpell->EffectBasePoints[1], masterSpell->EffectBasePoints[2]);
    }
    else
    {
        masterSpeed = 59;
        // use flying mounts in outland and northrend if no master
        if (!master && (bot->GetMapId() == 530 || bot->GetMapId() == 571))
        {
            if (bot->GetLevel() >= fourthmount)
                masterSpeed = 279;
            else if (bot->GetLevel() >= thirdmount)
                masterSpeed = 149;
        }
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
    if (bot->GetPureSkillValue(SKILL_RIDING) <= 75 && bot->GetLevel() < 60)
#endif
#ifdef CMANGOS
        if (bot->GetSkillValuePure(SKILL_RIDING) <= 75 && bot->GetLevel() < secondmount)
#endif
            masterSpeed = 59;

    if (bot->InBattleGround() && masterSpeed > 99)
        masterSpeed = 99;

    bool hasSwiftMount = false;

    for (PlayerSpellMap::iterator itr = bot->GetSpellMap().begin(); itr != bot->GetSpellMap().end(); ++itr)
    {
        uint32 spellId = itr->first;
        if (itr->second.state == PLAYERSPELL_REMOVED || itr->second.disabled || IsPassiveSpell(spellId))
            continue;

        const SpellEntry* spellInfo = sServerFacade.LookupSpellInfo(spellId);
        if (!spellInfo || spellInfo->EffectApplyAuraName[0] != SPELL_AURA_MOUNTED)
            continue;

        int32 effect = max(spellInfo->EffectBasePoints[1], spellInfo->EffectBasePoints[2]);
        //if (effect < masterSpeed)
        //    continue;

        uint32 index = 0;
#ifndef MANGOSBOT_ZERO
        index = (spellInfo->EffectApplyAuraName[1] == SPELL_AURA_MOD_FLIGHT_SPEED_MOUNTED ||
            spellInfo->EffectApplyAuraName[2] == SPELL_AURA_MOD_FLIGHT_SPEED_MOUNTED) ? 1 : 0;
#endif

        if (index == 0 && max(spellInfo->EffectBasePoints[1], spellInfo->EffectBasePoints[2]) > 59)
            hasSwiftMount = true;

        if (index == 1 && max(spellInfo->EffectBasePoints[1], spellInfo->EffectBasePoints[2]) > 59)
            hasSwiftMount = true;

        allSpells[index][effect].push_back(spellId);
    }


    int masterMountType = 0;
#ifndef MANGOSBOT_ZERO
    if (masterSpell != NULL)
    {
        masterMountType = (masterSpell->EffectApplyAuraName[1] == SPELL_AURA_MOD_FLIGHT_SPEED_MOUNTED ||
            masterSpell->EffectApplyAuraName[2] == SPELL_AURA_MOD_FLIGHT_SPEED_MOUNTED) ? 1 : 0;
    }

    // use flying mounts in outland and northrend if no master
    if (!master && (bot->GetMapId() == 530 || bot->GetMapId() == 571))
    {
        if (bot->GetLevel() >= fourthmount)
            masterMountType = 1;
        else if (bot->GetLevel() >= thirdmount)
            masterMountType = 1;
    }
#endif


    map<int32, vector<uint32> >& spells = allSpells[masterMountType];

    if (hasSwiftMount)
    {
        for (auto i : spells)
        {
            vector<uint32> ids = i.second;
            for (auto itr : ids)
            {
                const SpellEntry* spellInfo = sServerFacade.LookupSpellInfo(itr);
                if (!spellInfo)
                    continue;

                if (masterMountType == 0 && masterSpeed > 59 && max(spellInfo->EffectBasePoints[1], spellInfo->EffectBasePoints[2]) < 99)
                    spells[59].clear();

                if (masterMountType == 1 && masterSpeed > 59 && max(spellInfo->EffectBasePoints[1], spellInfo->EffectBasePoints[2]) < 279)
                    spells[59].clear();
            }
        }
    }

    for (map<int32, vector<uint32> >::iterator i = spells.begin(); i != spells.end(); ++i)
    {
        vector<uint32>& ids = i->second;
        int index = urand(0, ids.size() - 1);
        if (index >= ids.size())
            continue;

        MotionMaster& mm = *bot->GetMotionMaster();
        ai->CastSpell(ids[index], bot);
        return true;
    }

#ifndef MANGOSBOT_TWO
    list<Item*> items = AI_VALUE2(list<Item*>, "inventory items", "mount");

    vector<Item*> mounts(items.begin(), items.end());
    std::shuffle(mounts.begin(), mounts.end(), *GetRandomGenerator());

    Item* bestMount = nullptr;
    uint32 bestMountSpeed = 0;

#ifndef MANGOSBOT_ZERO
    if (bot->GetMapId() == 530 || bot->GetMapId() == 571)
        for (auto mount : mounts)
        {
            if (bot->CanUseItem(mount) != EQUIP_ERR_OK)
                continue;

            uint32 mountSpeed = MountSpeed(mount->GetProto(), true);

            if (mountSpeed < bestMountSpeed)
                continue;

            bestMount = mount;
            bestMountSpeed = mountSpeed;
        }
#endif
    if (!bestMount)
        for (auto mount : mounts)
        {
            if (bot->CanUseItem(mount) != EQUIP_ERR_OK)
                continue;

            uint32 mountSpeed = MountSpeed(mount->GetProto(), false);

            if (mountSpeed < bestMountSpeed)
                continue;

            bestMount = mount;
            bestMountSpeed = mountSpeed;
        }

    if (bestMountSpeed)
    {
        bool didUse = UseItemAuto(bestMount);

        if (didUse)
        {
            SetDuration(3000U); // 3s
            return true;
        }
    }
#endif

    return false;
}