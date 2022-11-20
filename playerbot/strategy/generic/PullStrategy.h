#pragma once

#include "../Strategy.h"
#include "../Multiplier.h"

namespace ai
{
    class PullStrategy : public Strategy
    {
    public:
        PullStrategy(PlayerbotAI* ai, string pullAction);

    public:
        void InitTriggers(std::list<TriggerNode*> &triggers) override;
        void InitMultipliers(std::list<Multiplier*> &multipliers) override;
        string getName() override { return "pull"; }

        static PullStrategy* Get(PlayerbotAI* ai);
        static uint8 GetMaxPullTime() { return 15; }
        const time_t& GetPullStartTime() const { return pullStartTime; }
        
        bool CanDoPullAction(Unit* target);

        Unit* GetTarget() const;
        bool HasTarget() const { return GetTarget() != nullptr; }

        string GetActionName() const;
        float GetRange() const { return range; }

        void RequestPull(Unit* target);
        bool IsPullPendingToStart() const { return pendingToStart; }
        bool HasPullStarted() const { return pullStartTime > 0; }
        void OnPullStarted();
        void OnPullEnded();

    private:
        void SetTarget(Unit* target);

    private:
        float range;
        string actionName;
        bool pendingToStart;
        time_t pullStartTime;
    };

    class PullMultiplier : public Multiplier
    {
    public:
        PullMultiplier(PlayerbotAI* ai) : Multiplier(ai, "pull") {}

    public:
        float GetValue(Action* action) override;
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
