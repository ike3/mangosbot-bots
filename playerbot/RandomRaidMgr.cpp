#include "../botpch.h"
#include "playerbot.h"
#include "PlayerbotAIConfig.h"
#include "RandomRaidMgr.h"
#include "PlayerbotAI.h"
#include "ServerFacade.h"

bool IsTank(Player* player)
{
    switch (player->getClass())
    {
    case CLASS_PALADIN:
    case CLASS_WARRIOR:
        return true;
    }
    return false;
}

bool IsHeal(Player* player)
{
    switch (player->getClass())
    {
    case CLASS_PRIEST:
    case CLASS_DRUID:
        return true;
    }
    return false;
}

RandomRaidMgr::~RandomRaidMgr()
{
    for(list<RandomRaid*>::iterator i = allianceRaids.begin(); i != allianceRaids.end(); ++i)
    {
        RandomRaid* raid = *i;
        delete raid;
    }
    allianceRaids.clear();

    for(list<RandomRaid*>::iterator i = hordeRaids.begin(); i != hordeRaids.end(); ++i)
    {
        RandomRaid* raid = *i;
        delete raid;
    }
    hordeRaids.clear();
}

int RandomRaid::GetBotCount(StrategyType type)
{
    int count = 0;
    for(vector<Player*>::iterator i = bots.begin(); i != bots.end(); ++i)
    {
        Player* bot = *i;
        bool tank = IsTank(bot);
        bool heal = IsHeal(bot);
        if (type == STRATEGY_TYPE_TANK && tank)
            count++;
        if (type == STRATEGY_TYPE_HEAL && heal)
            count++;
        if (type == STRATEGY_TYPE_DPS && !tank && !heal) count++;
    }
    return count;
}

void RandomRaid::Add(Player* bot)
{
    if (!bots.empty())
    {
        Player* leader = *bots.begin();

        WorldPacket p1;
        uint32 roles_mask = 0;
        p1 << leader->GetName();
        p1 << roles_mask;
        leader->GetSession()->HandleGroupInviteOpcode(p1);

        WorldPacket p2;
        p2 << roles_mask;
        bot->GetSession()->HandleGroupAcceptOpcode(p2);
    }
    bots.push_back(bot);
}

void RandomRaid::Remove(Player* bot)
{
    for(vector<Player*>::iterator i = bots.begin(); i != bots.end(); ++i)
    {
        Player* member = *i;
        if (member == bot)
        {
            member->RemoveFromGroup();
            break;
        }
    }
}

bool RandomRaid::IsFull()
{
    return GetBotCount(STRATEGY_TYPE_TANK) >= sPlayerbotAIConfig.randomRaidMaxTanks &&
            GetBotCount(STRATEGY_TYPE_HEAL) >= sPlayerbotAIConfig.randomRaidMaxHeals &&
            GetBotCount(STRATEGY_TYPE_DPS) >= sPlayerbotAIConfig.randomRaidMaxDps;
}

void RandomRaid::Attack()
{
    sRandomPlayerbotMgr.RandomTeleportForAttack(bots);
}

void RandomRaid::PrintStats()
{
    sLog.outString("%d members: %s", bots.size(), IsFull() ? "[full]" : "");

    for(vector<Player*>::iterator i = bots.begin(); i != bots.end(); ++i)
    {
        Player* bot = *i;
        PlayerbotAI *ai = bot->GetPlayerbotAI();
        sLog.outString("  - %s [%s]",
                bot->GetName(),
                IsTank(bot) ? "tank" : (IsHeal(bot) ? "heal" : "dps")
        );
    }
}

void RandomRaidMgr::Attack()
{
    for(list<RandomRaid*>::iterator i = allianceRaids.begin(); i != allianceRaids.end(); ++i)
    {
        RandomRaid* raid = *i;
        if (raid->IsFull()) raid->Attack();
    }

    for(list<RandomRaid*>::iterator i = hordeRaids.begin(); i != hordeRaids.end(); ++i)
    {
        RandomRaid* raid = *i;
        if (raid->IsFull()) raid->Attack();
    }
}

bool RandomRaidMgr::AddBot(Player* bot)
{
    PlayerbotAI *ai = bot->GetPlayerbotAI();

    RandomRaid* raid = NULL;
    if (IsTank(bot))
    {
        raid = AddBot(bot, STRATEGY_TYPE_TANK, sPlayerbotAIConfig.randomRaidMaxTanks);
    }
    else if (IsHeal(bot))
    {
        raid = AddBot(bot, STRATEGY_TYPE_HEAL, sPlayerbotAIConfig.randomRaidMaxHeals);
    }
    else
    {
        raid = AddBot(bot, STRATEGY_TYPE_DPS, sPlayerbotAIConfig.randomRaidMaxDps);
    }

    if (!raid) return false;

    if (raid->IsFull())
    {
        raid->Attack();
    }
    return true;
}

RandomRaid* RandomRaidMgr::AddBot(Player* bot, StrategyType type, int maxAlowed)
{
    list<RandomRaid*>& raids = GetRaidsFor(bot);
    for(list<RandomRaid*>::iterator i = raids.begin(); i != raids.end(); ++i)
    {
        RandomRaid* raid = *i;
        if (raid->GetBotCount(type) < maxAlowed)
        {
            raid->Add(bot);
            return raid;
        }
    }

    if (raids.size() >= sPlayerbotAIConfig.randomRaidMax)
        return NULL;

    RandomRaid* raid = new RandomRaid();
    raid->Add(bot);
    raids.push_back(raid);
    return raid;
}

list<RandomRaid*>& RandomRaidMgr::GetRaidsFor(Player* bot)
{
    return IsAlliance(bot->getRace()) ? allianceRaids : hordeRaids;
}

void RandomRaidMgr::RemoveBot(Player* bot)
{
    list<RandomRaid*>& raids = GetRaidsFor(bot);
    for(list<RandomRaid*>::iterator i = raids.begin(); i != raids.end(); ++i)
    {
        RandomRaid* raid = *i;
        raid->Remove(bot);
    }
}

void RandomRaidMgr::PrintStats()
{
    sLog.outString("Alliance raids");
    PrintStats(allianceRaids);

    sLog.outString("Horde raids");
    PrintStats(hordeRaids);
}

void RandomRaidMgr::PrintStats(list<RandomRaid*>& raids)
{
    for (list<RandomRaid*>::iterator i = raids.begin(); i != raids.end(); ++i)
    {
        RandomRaid* raid = *i;
        raid->PrintStats();
    }
}