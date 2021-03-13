#include "botpch.h"
#include "../../playerbot.h"
#include "LfgActions.h"
#include "../ItemVisitors.h"
//#include "../../AiFactory.h"
//#include "../../PlayerbotAIConfig.h"
//#include "../ItemVisitors.h"
//#include "../../RandomPlayerbotMgr.h"
//#include "../../../../game/LFGMgr.h"

using namespace ai;

bool LfgJoinAction::Execute(Event event)
{
    //if (bot->GetRestType() == REST_TYPE_NO)
    //    return false;

    if (ai->GetMaster() != bot)
    {
        ai->ChangeStrategy("-lfg", BOT_STATE_NON_COMBAT);
        return false;
    }

    if (bot->InBattleGround())
        return false;

    if (bot->InBattleGroundQueue())
        return false;

    if (!sPlayerbotAIConfig.randomBotJoinLfg)
        return false;

    if (bot->IsDead())
        return false;

    if (!sRandomPlayerbotMgr.IsRandomBot(bot))
        return false;

#ifdef MANGOSBOT_TWO

    if (sLFGMgr.GetQueueInfo(bot->GetObjectGuid()))
        return false;

    LFGPlayerState* pState = sLFGMgr.GetLFGPlayerState(bot->GetObjectGuid());
    if (pState->GetState() != LFG_STATE_NONE)
        return false;

    if (bot->IsBeingTeleported())
        return false;

    Map* map = bot->GetMap();
    if (map && map->Instanceable())
        return false;

    return JoinLFG();

#else
    return false; // TODO Vanilla + TBC LFG
#endif
}

#ifdef MANGOSBOT_TWO
LFGRoleMask LfgJoinAction::GetRoles()
{
    if (!sRandomPlayerbotMgr.IsRandomBot(bot))
    {
        if (ai->IsTank(bot))
            return LFG_ROLE_MASK_TANK;
        if (ai->IsHeal(bot))
            return LFG_ROLE_MASK_HEALER;
        else
            return LFG_ROLE_MASK_DAMAGE;
    }

    int spec = AiFactory::GetPlayerSpecTab(bot);
    switch (bot->getClass())
    {
    case CLASS_DRUID:
        if (spec == 2)
            return LFG_ROLE_MASK_HEALER;
        else if (spec == 1 && bot->getLevel() >= 20)
            return LFG_ROLE_MASK_TANK_DAMAGE;
        else
            return LFG_ROLE_MASK_DAMAGE;
        break;
    case CLASS_PALADIN:
        if (spec == 1)
            return LFG_ROLE_MASK_TANK;
        else if (spec == 0)
            return LFG_ROLE_MASK_HEALER;
        else
            return LFG_ROLE_MASK_DAMAGE;
        break;
    case CLASS_PRIEST:
        if (spec != 2)
            return LFG_ROLE_MASK_HEALER;
        else
            return LFG_ROLE_MASK_DAMAGE;
        break;
    case CLASS_SHAMAN:
        if (spec == 2)
            return LFG_ROLE_MASK_HEALER;
        else
            return LFG_ROLE_MASK_DAMAGE;
        break;
    case CLASS_WARRIOR:
        if (spec == 2)
            return LFG_ROLE_MASK_TANK;
        else
            return LFG_ROLE_MASK_DAMAGE;
        break;
#ifdef MANGOSBOT_TWO
    case CLASS_DEATH_KNIGHT:
        if (spec == 0)
            return LFG_ROLE_MASK_TANK;
        else
            return LFG_ROLE_MASK_DAMAGE;
        break;
#endif
    default:
        return LFG_ROLE_MASK_DAMAGE;
        break;
    }

    return LFG_ROLE_MASK_DAMAGE;
}
#endif

bool LfgJoinAction::SetRoles()
{
#ifdef MANGOSBOT_TWO
    LFGPlayerState* pState = sLFGMgr.GetLFGPlayerState(bot->GetObjectGuid());
    if (!pState)
        return false;

    pState->SetRoles(GetRoles());
#endif

    return true;
}

bool LfgJoinAction::JoinLFG()
{
#ifdef MANGOSBOT_TWO
    LFGPlayerState* pState = sLFGMgr.GetLFGPlayerState(bot->GetObjectGuid());

    // check if already in lfg
    if (pState->GetDungeons() && !pState->GetDungeons()->empty())
        return false;

    // set roles
    if (!SetRoles())
        return false;

    ItemCountByQuality visitor;
    IterateItems(&visitor, ITERATE_ITEMS_IN_EQUIP);
    bool random = urand(0, 100) < 50;
    bool heroic = urand(0, 100) < 50 && (visitor.count[ITEM_QUALITY_EPIC] >= 3 || visitor.count[ITEM_QUALITY_RARE] >= 10) && bot->getLevel() >= 70;
    bool raid = !heroic && (urand(0, 100) < 50 && visitor.count[ITEM_QUALITY_EPIC] >= 5 && (bot->getLevel() == 60 || bot->getLevel() == 70 || bot->getLevel() == 80));

    LFGDungeonSet list;
    vector<uint32> idx;
    for (uint32 i = 0; i < sLFGDungeonStore.GetNumRows(); ++i)
    {
        LFGDungeonEntry const* dungeon = sLFGDungeonStore.LookupEntry(i);
        if (!dungeon || (dungeon->type != LFG_TYPE_RANDOM_DUNGEON && dungeon->type != LFG_TYPE_DUNGEON && dungeon->type != LFG_TYPE_HEROIC_DUNGEON &&
            dungeon->type != LFG_TYPE_RAID))
            continue;

        uint32 botLevel = bot->getLevel();
        if (dungeon->minlevel && botLevel < dungeon->minlevel)
            continue;

        if (dungeon->minlevel && botLevel > dungeon->minlevel + 10)
            continue;

        if (dungeon->maxlevel && botLevel > dungeon->maxlevel)
            continue;

        if (heroic && !dungeon->difficulty)
            continue;

        if (raid && dungeon->type != LFG_TYPE_RAID)
            continue;

        if (!random && !raid && !heroic && dungeon->type != LFG_TYPE_DUNGEON)
            continue;

        list.insert(dungeon);
    }

    if (list.empty() && !random)
        return false;

    // check role for console msg
    string _roles = "multiple roles";

    if (pState->HasRole(ROLE_TANK))
        _roles = "TANK";
    if (pState->HasRole(ROLE_HEALER))
        _roles = "HEAL";
    if (pState->HasRole(ROLE_DAMAGE))
        _roles = "DPS";

    /*if (lfgState->IsSingleRole())
    {
        if (lfgState->HasRole(ROLE_TANK))
            _roles = "TANK";
        if (lfgState->HasRole(ROLE_HEALER))
            _roles = "HEAL";
        if (lfgState->HasRole(ROLE_DAMAGE))
            _roles = "DPS";
    }*/

    LFGDungeonEntry const* dungeon;

    if(!random)
        dungeon = *list.begin();

    bool many = list.size() > 1;

    if (random)
    {
        LFGDungeonSet randList = sLFGMgr.GetRandomDungeonsForPlayer(bot);
        for (LFGDungeonSet::const_iterator itr = randList.begin(); itr != randList.end(); ++itr)
        {
            LFGDungeonEntry const* dungeon = *itr;

            if (!dungeon)
                continue;

            idx.push_back(dungeon->ID);
        }
        if (idx.empty())
            return false;

        // choose random dungeon
        dungeon = sLFGDungeonStore.LookupEntry(idx[urand(0, idx.size() - 1)]);
        list.insert(dungeon);

        pState->SetType(LFG_TYPE_RANDOM_DUNGEON);
        sLog.outBasic("Bot #%d %s:%d <%s>: queues LFG, Random Dungeon as %s (%s)", bot->GetGUIDLow(), bot->GetTeam() == ALLIANCE ? "A" : "H", bot->getLevel(), bot->GetName(), _roles, dungeon->name[0]);
        return true;
    }
    else if (heroic)
    {
        pState->SetType(LFG_TYPE_HEROIC_DUNGEON);
        sLog.outBasic("Bot #%d %s:%d <%s>: queues LFG, Heroic Dungeon as %s (%s)", bot->GetGUIDLow(), bot->GetTeam() == ALLIANCE ? "A" : "H", bot->getLevel(), bot->GetName(), _roles, many ? "several dungeons" : dungeon->name[0]);
    }
    else if (raid)
    {
        pState->SetType(LFG_TYPE_RAID);
        sLog.outBasic("Bot #%d  %s:%d <%s>: queues LFG, Raid as %s (%s)", bot->GetGUIDLow(), bot->GetTeam() == ALLIANCE ? "A" : "H", bot->getLevel(), bot->GetName(), _roles, many ? "several dungeons" : dungeon->name[0]);
    }
    else
    {
        pState->SetType(LFG_TYPE_DUNGEON);
        sLog.outBasic("Bot #%d %s:%d <%s>: queues LFG, Dungeon as %s (%s)", bot->GetGUIDLow(), bot->GetTeam() == ALLIANCE ? "A" : "H", bot->getLevel(), bot->GetName(), _roles, many ? "several dungeons" : dungeon->name[0]);
    }

    // Set Raid Browser comment
    string _gs = to_string(bot->GetEquipGearScore());
    pState->SetComment("Bot " + _roles + " GS:" + _gs + " for LFG");


    pState->SetDungeons(list);
    sLFGMgr.Join(bot);

#endif
    return true;
}

bool LfgRoleCheckAction::Execute(Event event)
{
#ifdef MANGOSBOT_TWO
    Group* group = bot->GetGroup();
    if (group)
    {
        LFGPlayerState* pState = sLFGMgr.GetLFGPlayerState(bot->GetObjectGuid());
        LFGRoleMask currentRoles = pState->GetRoles();
        LFGRoleMask newRoles = GetRoles();
        if (currentRoles == newRoles) return false;
        
        pState->SetRoles(newRoles);

        //sLFGMgr.UpdateRoleCheck(group);

        sLog.outBasic("Bot #%d %s:%d <%s>: LFG roles checked", bot->GetGUIDLow(), bot->GetTeam() == ALLIANCE ? "A" : "H", bot->getLevel(), bot->GetName());

        return true;
    }
#endif

    return false;
}

bool LfgAcceptAction::Execute(Event event)
{
#ifdef MANGOSBOT_TWO
    LFGPlayerState* pState = sLFGMgr.GetLFGPlayerState(bot->GetObjectGuid());
    if (!pState || pState->GetState() != LFG_STATE_PROPOSAL)
        return false;

    uint32 id = AI_VALUE(uint32, "lfg proposal");
    if (id)
    {
        //if (urand(0, 1 + 10 / sPlayerbotAIConfig.randomChangeMultiplier))
        //    return false;

        if (bot->IsInCombat() || bot->IsDead())
        {
            sLog.outBasic("Bot #%d %s:%d <%s> is in combat and refuses LFG proposal %d", bot->GetGUIDLow(), bot->GetTeam() == ALLIANCE ? "A" : "H", bot->getLevel(), bot->GetName(), id);
            sLFGMgr.UpdateProposal(id, bot->GetObjectGuid(), false);
            return true;
        }

        sLog.outBasic("Bot #%d %s:%d <%s> accepts LFG proposal %d", bot->GetGUIDLow(), bot->GetTeam() == ALLIANCE ? "A" : "H", bot->getLevel(), bot->GetName(), id);
        ai->GetAiObjectContext()->GetValue<uint32>("lfg proposal")->Set(0);
        bot->clearUnitState(UNIT_STAT_ALL_STATE);
        sLFGMgr.UpdateProposal(id, bot->GetObjectGuid(), true);

        if (sRandomPlayerbotMgr.IsRandomBot(bot) && !bot->GetGroup())
        {
            sRandomPlayerbotMgr.Refresh(bot);
            ai->ResetStrategies();
            //bot->TeleportToHomebind();
        }

        //ai->ChangeStrategy("-grind,-rpg,-travel", BOT_STATE_NON_COMBAT);
        ai->Reset();

        return true;
    }

    WorldPacket p(event.getPacket());

    uint32 dungeon;
    uint8 state;
    p >> dungeon >> state >> id;

    ai->GetAiObjectContext()->GetValue<uint32>("lfg proposal")->Set(id);
#endif
    return true;
}

bool LfgLeaveAction::Execute(Event event)
{
#ifdef MANGOSBOT_TWO
    // Don't leave if lfg strategy enabled
    if (ai->HasStrategy("lfg", BOT_STATE_NON_COMBAT))
        return false;

    // Don't leave if already invited / in dungeon
    if (sLFGMgr.GetLFGPlayerState(bot->GetObjectGuid())->GetState() > LFG_STATE_QUEUED)
        return false;

    sLFGMgr.Leave(bot);
#endif
    return true;
}

bool LfgTeleportAction::Execute(Event event)
{
#ifdef MANGOSBOT_TWO
    bool out = false;

    WorldPacket p(event.getPacket());
    if (!p.empty())
    {
        p.rpos(0);
        p >> out;
    }

    bot->clearUnitState(UNIT_STAT_ALL_STATE);
    sLFGMgr.Teleport(bot, out);
#endif
    return true;
}
