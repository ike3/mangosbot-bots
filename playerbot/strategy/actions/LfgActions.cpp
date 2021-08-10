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
    return JoinLFG();
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
#ifdef MANGOSBOT_ZERO
    //ItemCountByQuality visitor;
    //IterateItems(&visitor, ITERATE_ITEMS_IN_EQUIP);
    //bool raid = (urand(0, 100) < 50 && visitor.count[ITEM_QUALITY_EPIC] >= 5 && (bot->getLevel() == 60 || bot->getLevel() == 70 || bot->getLevel() == 80));

    MeetingStoneSet stones = sLFGMgr.GetDungeonsForPlayer(bot);
    if (!stones.size())
        return false;

    vector<uint32> dungeons = sRandomPlayerbotMgr.LfgDungeons[bot->GetTeam()];
    if (!dungeons.size())
        return false;

    vector<MeetingStoneInfo> selected;

    for (MeetingStoneSet::iterator i = stones.begin(); i != stones.end(); ++i)
    {
        vector<uint32>::iterator it = find(dungeons.begin(), dungeons.end(), i->area);
        if (it != dungeons.end())
            selected.push_back(*i);
    }

    if (!selected.size())
        return false;

    uint32 dungeon = urand(0, selected.size() - 1);
    MeetingStoneInfo stoneInfo = selected[dungeon];
    BotRoles botRoles = AiFactory::GetPlayerRoles(bot);
    string _botRoles;
    switch (botRoles)
    {
    case BOT_ROLE_TANK:
        _botRoles = "Tank";
        break;
    case BOT_ROLE_HEALER:
        _botRoles = "Healer";
        break;
    case BOT_ROLE_DPS:
    default:
        _botRoles = "Dps";
        break;
    }
    /*for (MeetingStoneSet::const_iterator itr = stones.begin(); itr != stones.end(); ++itr)
    {
        auto data = *itr;

        idx.push_back(data.area);
    }

    if (idx.empty())
        return false;*/

    sLog.outBasic("Bot #%d %s:%d <%s>: queues LFG to %s as %s", bot->GetGUIDLow(), bot->GetTeam() == ALLIANCE ? "A" : "H", bot->getLevel(), bot->GetName(), stoneInfo.name, _botRoles);

    sLFGMgr.AddToQueue(bot, stoneInfo.area);
#endif
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
    bool random = urand(0, 100) < 20;
    bool heroic = urand(0, 100) < 50 && (visitor.count[ITEM_QUALITY_EPIC] >= 3 || visitor.count[ITEM_QUALITY_RARE] >= 10) && bot->getLevel() >= 70;
    bool raid = !heroic && (urand(0, 100) < 50 && visitor.count[ITEM_QUALITY_EPIC] >= 5 && (bot->getLevel() == 60 || bot->getLevel() == 70 || bot->getLevel() == 80));

    LFGDungeonSet list;
    vector<uint32> selected;

    vector<uint32> dungeons = sRandomPlayerbotMgr.LfgDungeons[bot->GetTeam()];
    if (!dungeons.size())
        return false;

    for (vector<uint32>::iterator i = dungeons.begin(); i != dungeons.end(); ++i)
    {
        LFGDungeonEntry const* dungeon = sLFGDungeonStore.LookupEntry(*i);
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

        selected.push_back(dungeon->ID);
        list.insert(dungeon);
    }

    if (!selected.size())
        return false;

    if (list.empty())
        return false;

    bool many = list.size() > 1;
    LFGDungeonEntry const* dungeon = *list.begin();

    /*for (uint32 i = 0; i < sLFGDungeonStore.GetNumRows(); ++i)
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
        return false;*/

    // check role for console msg
    string _roles = "multiple roles";

    if (pState->HasRole(ROLE_TANK))
        _roles = "TANK";
    if (pState->HasRole(ROLE_HEALER))
        _roles = "HEAL";
    if (pState->HasRole(ROLE_DAMAGE))
        _roles = "DPS";

    pState->SetType(LFG_TYPE_DUNGEON);
    sLog.outBasic("Bot #%d %s:%d <%s>: queues LFG, Dungeon as %s (%s)", bot->GetGUIDLow(), bot->GetTeam() == ALLIANCE ? "A" : "H", bot->getLevel(), bot->GetName(), _roles, many ? "several dungeons" : dungeon->name[0]);

    /*if (lfgState->IsSingleRole())
    {
        if (lfgState->HasRole(ROLE_TANK))
            _roles = "TANK";
        if (lfgState->HasRole(ROLE_HEALER))
            _roles = "HEAL";
        if (lfgState->HasRole(ROLE_DAMAGE))
            _roles = "DPS";
    }*/

    /*LFGDungeonEntry const* dungeon;

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
    }*/

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

        sLFGMgr.UpdateRoleCheck(group);

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
    // Don't leave if lfg strategy enabled
    //if (ai->HasStrategy("lfg", BOT_STATE_NON_COMBAT))
    //    return false;
#ifdef MANGOSBOT_ZERO
    LFGPlayerQueueInfo qInfo;
    sLFGMgr.GetPlayerQueueInfo(&qInfo, bot->GetObjectGuid());
    AreaTableEntry const* area = GetAreaEntryByAreaID(qInfo.areaId);
    if (area)
    {
        sLog.outBasic("Bot #%d %s:%d <%s>: leaves LFG queue to %s after %u minutes", bot->GetGUIDLow(), bot->GetTeam() == ALLIANCE ? "A" : "H", bot->getLevel(), bot->GetName(), area->area_name[0], (qInfo.timeInLFG / 60000));
        sLFGMgr.RemovePlayerFromQueue(bot->GetObjectGuid(), PLAYER_CLIENT_LEAVE);
    }
#endif
#ifdef MANGOSBOT_TWO
    // Don't leave if already invited / in dungeon
    if (sLFGMgr.GetLFGPlayerState(bot->GetObjectGuid())->GetState() > LFG_STATE_QUEUED)
        return false;

    sLFGMgr.Leave(bot);
#endif
    return true;
}

bool LfgLeaveAction::isUseful()
{
#ifdef MANGOSBOT_ZERO
    if (!sLFGMgr.IsPlayerInQueue(bot->GetObjectGuid()))
        return false;
    else
    {
        LFGPlayerQueueInfo qInfo;
        sLFGMgr.GetPlayerQueueInfo(&qInfo, bot->GetObjectGuid());
        if (qInfo.timeInLFG < (5 * MINUTE * IN_MILLISECONDS))
            return false;
    }

    if (bot->GetGroup() && bot->GetGroup()->GetLeaderGuid() != bot->GetObjectGuid())
    {
        if (sLFGMgr.IsPlayerInQueue(bot->GetGroup()->GetLeaderGuid()))
            return false;
    }

    if ((ai->GetMaster() && !ai->GetMaster()->GetPlayerbotAI()))
    {
        return false;
    }
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

bool LfgJoinAction::isUseful()
{
    if (!sPlayerbotAIConfig.randomBotJoinLfg)
    {
        //ai->ChangeStrategy("-lfg", BOT_STATE_NON_COMBAT);
        return false;
    }

    if (bot->getLevel() < 15)
        return false;

    if ((ai->GetMaster() && !ai->GetMaster()->GetPlayerbotAI()) || bot->GetGroup() && bot->GetGroup()->GetLeaderGuid() != bot->GetObjectGuid())
    {
        //ai->ChangeStrategy("-lfg", BOT_STATE_NON_COMBAT);
        return false;
    }

    if (bot->IsBeingTeleported())
        return false;

    if (bot->InBattleGround())
        return false;

    if (bot->InBattleGroundQueue())
        return false;

    if (bot->IsDead())
        return false;

    if (!sRandomPlayerbotMgr.IsRandomBot(bot))
        return false;

    Map* map = bot->GetMap();
    if (map && map->Instanceable())
        return false;
    
#ifdef MANGOSBOT_ZERO
    if (sRandomPlayerbotMgr.LfgDungeons[bot->GetTeam()].empty())
        return false;

    if (sLFGMgr.IsPlayerInQueue(bot->GetObjectGuid()))
        return false;

    BotRoles botRoles = AiFactory::GetPlayerRoles(bot);

    RolesPriority prio = sLFGMgr.getPriority((Classes)bot->getClass(), (ClassRoles)botRoles);
    if (prio < LFG_PRIORITY_NORMAL)
        return false;

    if (bot->GetGroup() && bot->GetGroup()->IsFull())
        return false;
#endif
#ifdef MANGOSBOT_TWO

    if (sLFGMgr.GetQueueInfo(bot->GetObjectGuid()))
        return false;

    LFGPlayerState* pState = sLFGMgr.GetLFGPlayerState(bot->GetObjectGuid());
    if (pState->GetState() != LFG_STATE_NONE)
        return false;
#endif
    return true;
}
