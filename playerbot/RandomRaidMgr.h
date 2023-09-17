#ifndef _RandomRaidMgr_H
#define _RandomRaidMgr_H

#include "Common.h"
#include "PlayerbotAIBase.h"

using namespace std;

class RandomRaid
{
public:
    RandomRaid() {}

    int GetBotCount(StrategyType type);
    void Add(Player* bot);
    void Remove(Player* bot);
    bool IsFull();
    void Attack();
    void PrintStats();

private:
    vector<Player*> bots;
};

class RandomRaidMgr
{
    public:
        RandomRaidMgr() {}
        virtual ~RandomRaidMgr();
        static RandomRaidMgr& instance()
        {
            static RandomRaidMgr instance;
            return instance;
        }

	public:
        bool AddBot(Player* bot);
        void RemoveBot(Player* bot);
        void PrintStats();
        void Attack();

	private:
        RandomRaid* AddBot(Player* bot, StrategyType type, int maxAlowed);
        list<RandomRaid*>& GetRaidsFor(Player* bot);
        void PrintStats(list<RandomRaid*>& raids);

    private:
        list<RandomRaid*> allianceRaids;
        list<RandomRaid*> hordeRaids;
};

#define sRandomRaidMgr RandomRaidMgr::instance()

#endif
