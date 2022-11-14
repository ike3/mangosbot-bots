#pragma once

#include "../Strategy.h"
#include "../Multiplier.h"

namespace ai
{
    class PullStrategy : public Strategy
    {
    public:
        PullStrategy(PlayerbotAI* ai, string pullAction, float pullRange);

    public:
        void InitTriggers(std::list<TriggerNode*> &triggers) override;
        void InitMultipliers(std::list<Multiplier*> &multipliers) override;
        string getName() override { return "pull"; }

        static PullStrategy* Get(PlayerbotAI* ai);
        static uint8 GetMaxPullTime() { return 15; }

        Unit* GetTarget() const;
        void SetTarget(Unit* target);

        bool CanPull(Unit* target) const;
        bool IsPulling() const { return GetTarget() != nullptr; }

        string GetAction() const;
        float GetRange() const { return range; }

        bool HasPullStarted() const { return started; }
        void OnPullStart() { started = true; }
        void OnPullEnd() { started = false; }

    private:
        string action;
        float range;
        bool started;
    };

    class PullMultiplier : public Multiplier
    {
    public:
        PullMultiplier(PlayerbotAI* ai, string pullAction) : Multiplier(ai, "pull"), pullAction(pullAction) {}

    public:
        float GetValue(Action* action) override;

    private:
        string pullAction;
    };

    class PossibleAdsStrategy : public Strategy
    {
    public:
        PossibleAdsStrategy(PlayerbotAI* ai) : Strategy(ai) {}

    public:
        virtual void InitTriggers(std::list<TriggerNode*> &triggers);
        virtual string getName() { return "ads"; }
    };
}
