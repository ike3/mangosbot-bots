#pragma once
#include "botpch.h"
#include "../../playerbot.h"
#include "../../Talentspec.h"
#include "../Action.h"

namespace ai
{
    class ChangeTalentsAction : public Action {
    public:
        ChangeTalentsAction(PlayerbotAI* ai, string name = "talents") : Action(ai, name) {}

    public:
        virtual bool Execute(Event& event);
        virtual bool AutoSelectTalents(ostringstream* out);
    private:
        std::vector<TalentPath*> getPremadePaths(string findName);
        std::vector<TalentPath*> getPremadePaths(TalentSpec* oldSpec);
        TalentPath* getPremadePath(int id);
        void listPremadePaths(std::vector<TalentPath*> paths, ostringstream* out);
        TalentPath* PickPremadePath(std::vector<TalentPath*> paths, bool useProbability);
        TalentSpec* GetBestPremadeSpec(int spec);
    };
    class AutoSetTalentsAction : public ChangeTalentsAction {
    public:
        AutoSetTalentsAction(PlayerbotAI* ai) : ChangeTalentsAction(ai, "auto talents") {}
        virtual bool Execute(Event& event);
    };
}