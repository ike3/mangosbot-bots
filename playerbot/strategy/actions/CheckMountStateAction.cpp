#include "botpch.h"
#include "../../playerbot.h"
#include "CheckMountStateAction.h"

#include "../../ServerFacade.h"
#include "BattleGroundWS.h"
using namespace ai;

uint64 extractGuid(WorldPacket& packet);

bool CheckMountStateAction::Execute(Event event)
{
    if (bot->IsDead())
        return false;

    bool isOutdoor;
    uint16 areaFlag = bot->GetMap()->GetTerrain()->GetAreaFlag(bot->GetPositionX(), bot->GetPositionY(), bot->GetPositionZ(), &isOutdoor);
    if (!isOutdoor)
        return false;

    if (bot->IsTaxiFlying())
        return false;

    if (!bot->GetPlayerbotAI()->HasStrategy("mount", BOT_STATE_NON_COMBAT))
        return false;

    if (bot->isMoving())
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

    Player* master = GetMaster();
    if (master != nullptr && !bot->InBattleGround())
    {
        if (!bot->GetGroup() || bot->GetGroup()->GetLeaderGuid() != master->GetObjectGuid())
            return false;

        bool farFromMaster = sServerFacade.GetDistance2d(bot, master) > sPlayerbotAIConfig.sightDistance;
        if (master->IsMounted() && !bot->IsMounted())
        {
            return Mount();
        }
        if ((farFromMaster || !master->IsMounted()) && bot->IsMounted())
        {
            WorldPacket emptyPacket;
            bot->GetSession()->HandleCancelMountAuraOpcode(emptyPacket);
            return true;
        }
        return false;
    }

    list<ObjectGuid> targets = AI_VALUE(list<ObjectGuid>, "possible targets");
    bool noattackers = (targets.size() > 0 || bot->getAttackers().size() > 0) ? false : true;

    if (bot->InBattleGround() && noattackers && !bot->IsInCombat() && !bot->IsMounted())
    {
        if (bot->GetBattleGroundTypeId() == BattleGroundTypeId::BATTLEGROUND_WS)
        {
            BattleGroundWS *bg = (BattleGroundWS*)ai->GetBot()->GetBattleGround();
            //if (!(bg->GetFlagState(bg->GetOtherTeam(bot->GetTeam())) == BG_WS_FLAG_STATE_ON_PLAYER))
            //    return Mount();
            if (bot->GetObjectGuid() == bg->GetAllianceFlagCarrierGuid() || bot->GetObjectGuid() == bg->GetHordeFlagCarrierGuid())
            {
                return false;
            }
            else
                return Mount();
        }
    }

    ObjectGuid unit = AI_VALUE(ObjectGuid, "rpg target");
    if (unit)
    {
        float distance = AI_VALUE2(float, "distance", "rpg target");
        if (distance > sPlayerbotAIConfig.farDistance && noattackers)
            return Mount();
    }

    if (((!AI_VALUE(list<ObjectGuid>, "possible rpg targets").empty()) || urand(0, 100) > 50) && noattackers)
        return Mount();

    bool attackdistance = false;
    bool chasedistance = false;
    Unit* target = bot->GetMap()->GetUnit(bot->GetTargetGuid());

    if (target && target->IsHostileTo(bot))
    {
        attackdistance = sServerFacade.GetDistance2d(bot, target) <= sPlayerbotAIConfig.spellDistance;
        chasedistance = sServerFacade.GetDistance2d(bot, target) >= sPlayerbotAIConfig.fleeDistance;
    }

    if ((attackdistance || bot->IsInCombat()) && bot->IsMounted()/* && !bot->GetPlayerbotAI()->HasStrategy("mount", BOT_STATE_NON_COMBAT)*/)
    {
        WorldPacket emptyPacket;
        bot->GetSession()->HandleCancelMountAuraOpcode(emptyPacket);
        return true;
    }

    /*if (chasedistance && !bot->IsMounted() && !target->IsMounted() && !bot->IsInCombat() && !target->IsInCombat() && target->IsHostileTo(bot))
    {
        return Mount();
    }*/

    return false;
}

bool CheckMountStateAction::Mount()
{
	Player* master = GetMaster();
	ai->RemoveShapeshift();

    int32 masterSpeed = 150;
    const SpellEntry *masterSpell = NULL;

    if (master != NULL && master->GetAurasByType(SPELL_AURA_MOUNTED).size() > 0)
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

	map<int32, vector<uint32> > spells;
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

		spells[effect].push_back(spellId);
	}

#ifndef MANGOSBOT_ZERO
    int masterMountType = 0;
    if (masterSpell != NULL)
    {
        masterMountType = (masterSpell->EffectApplyAuraName[1] == SPELL_AURA_MOD_INCREASE_MOUNTED_FLIGHT_SPEED ||
            masterSpell->EffectApplyAuraName[2] == SPELL_AURA_MOD_INCREASE_MOUNTED_FLIGHT_SPEED) ? 1 : 0;
    }
#endif

	for (map<int32, vector<uint32> >::iterator i = spells.begin(); i != spells.end(); ++i)
	{
		vector<uint32>& ids = i->second;
		int index = urand(0, ids.size() - 1);
		if (index >= ids.size())
			continue;

        MotionMaster &mm = *bot->GetMotionMaster();
        
        //bot->GetMotionMaster()->MovementExpired();
        //bot->StopMoving(true);
        //bot->SetRoot(true);
        //ai->SetNextCheckDelay(3100);
        //bot->StopMoving();
        //ai->SetNextCheckDelay(3000);
		ai->CastSpell(ids[index], bot);
        
        
        //bot->SetRoot(false);
		return true;
	}

    list<Item*> items = AI_VALUE2(list<Item*>, "inventory items", "mount");
    if (!items.empty()) return UseItemAuto(*items.begin());

    return false;
}
