#pragma once
#include "../Multiplier.h"
#include "../Strategy.h"

namespace ai
{
    class ConserveManaMultiplier : public Multiplier
    {
    public:
        ConserveManaMultiplier(PlayerbotAI* ai) : Multiplier(ai, "conserve mana") {}

    public:
        virtual float GetValue(Action* action);
    };

    class SaveManaMultiplier : public Multiplier
    {
    public:
        SaveManaMultiplier(PlayerbotAI* ai) : Multiplier(ai, "save mana") {}

    public:
        virtual float GetValue(Action* action);
    };

    class ConserveManaStrategy : public Strategy
    {
    public:
        ConserveManaStrategy(PlayerbotAI* ai) : Strategy(ai) {}
        string getName() override { return "conserve mana"; }

#ifndef GenerateBotHelp
        virtual string GetHelpName() { return "conserve mana"; } //Must equal iternal name
        virtual string GetHelpDescription() {
            return "This strategy will make bots wait longer between casting the same spell twice.\n"
                   "the delay is based on [h:value|mana save level].";
        }
        virtual vector<string> GetRelatedStrategies() { return { }; }
#endif
    private:
        void InitCombatMultipliers(std::list<Multiplier*> &multipliers) override;
    };
}
