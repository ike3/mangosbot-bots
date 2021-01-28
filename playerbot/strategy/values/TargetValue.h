#pragma once
#include "../Value.h"
#include "../../TravelMgr.h"

namespace ai
{
    class FindTargetStrategy
    {
    public:
        FindTargetStrategy(PlayerbotAI* ai)
        {
            result = NULL;
            this->ai = ai;
        }

    public:
        Unit* GetResult() { return result; }

    public:
        virtual void CheckAttacker(Unit* attacker, ThreatManager* threatManager) = 0;
        void GetPlayerCount(Unit* creature, int* tankCount, int* dpsCount);

    protected:
        Unit* result;
        PlayerbotAI* ai;

    protected:
        map<Unit*, int> tankCountCache;
        map<Unit*, int> dpsCountCache;
    };

    class FindNonCcTargetStrategy : public FindTargetStrategy
    {
    public:
        FindNonCcTargetStrategy(PlayerbotAI* ai) : FindTargetStrategy(ai) {}

    protected:
        virtual bool IsCcTarget(Unit* attacker);

    };

    class TargetValue : public UnitCalculatedValue
	{
	public:
        TargetValue(PlayerbotAI* ai) : UnitCalculatedValue(ai) {}

    protected:
        Unit* FindTarget(FindTargetStrategy* strategy);
    };

    class RpgTargetValue : public ManualSetValue<ObjectGuid>
    {
    public:
        RpgTargetValue(PlayerbotAI* ai) : ManualSetValue<ObjectGuid>(ai, ObjectGuid()) {}
    };
	
    class TravelTargetValue : public ManualSetValue<TravelTarget *>
    {
    public:
        TravelTargetValue(PlayerbotAI* ai) : ManualSetValue<TravelTarget*>(ai, new TravelTarget(ai)) {}
        virtual ~TravelTargetValue() { delete value; }
    };	

    class IgnoreRpgTargetValue : public ManualSetValue<set<ObjectGuid>& >
    {
    public:
        IgnoreRpgTargetValue(PlayerbotAI* ai) : ManualSetValue<set<ObjectGuid>& >(ai, data, "ignore rpg targets") {}

    private:
        set<ObjectGuid> data;
    };

    class TalkTargetValue : public ManualSetValue<ObjectGuid>
    {
    public:
        TalkTargetValue(PlayerbotAI* ai) : ManualSetValue<ObjectGuid>(ai, ObjectGuid()) {}
    };

    class PullTargetValue : public ManualSetValue<ObjectGuid>
    {
    public:
        PullTargetValue(PlayerbotAI* ai) : ManualSetValue<ObjectGuid>(ai, ObjectGuid()) {}
    };
}
