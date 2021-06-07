#pragma once
#include "../Value.h"

namespace ai
{
    enum class QuestRelationType : uint32
    {
        objective1 = 0,
        objective2 = 1,
        objective3 = 2,
        objective4 = 3,
        questGiver = 4,
        questTaker = 5
    };

    typedef unordered_map < QuestRelationType, list<GuidPosition>> questGuidps;
    typedef unordered_map < uint32, questGuidps> questGuidpMap;

    typedef unordered_map < uint32, list<GuidPosition>> questGivers;

    //Cheat class copy to hack into the loot system
    class LootTemplateAccess
    {
    public:
        class  LootGroup;                                   // A set of loot definitions for items (refs are not allowed inside)
        typedef std::vector<LootGroup> LootGroups;
        LootStoreItemList Entries;                          // not grouped only
        LootGroups        Groups;                           // groups have own (optimized) processing, grouped entries go there
    };

    //Generic quest object finder
    class FindQuestObjectData
    {
    public:
        FindQuestObjectData() { GetObjectiveEntries(); }

        void GetObjectiveEntries();
        bool operator()(CreatureDataPair const& dataPair);
        bool operator()(GameObjectDataPair const& dataPair);
        questGuidpMap GetResult() const { return data; };
    private:
        unordered_map<int32, vector<pair<uint32, QuestRelationType>>> entryMap;
        unordered_map<uint32, vector<pair<uint32, QuestRelationType>>> itemMap;

        questGuidpMap data;
    };

    //All objects to start, do or finish a quest.
    class QuestGuidpMapValue : public SingleCalculatedValue<questGuidpMap>
    {
    public:
        QuestGuidpMapValue(PlayerbotAI* ai) : SingleCalculatedValue(ai, "quest guidp map") {}

        virtual questGuidpMap Calculate();
    };

    //All questgivers and their quests that are usefull for a specific level
    class QuestGiversValue : public SingleCalculatedValue<questGivers>, public Qualified
	{
	public:
        QuestGiversValue(PlayerbotAI* ai) : SingleCalculatedValue(ai, "quest givers") {}

        virtual questGivers Calculate();
    };

    //All questgivers that have a quest for the bot.
    class ActiveQuestGiversValue : public CalculatedValue<list<GuidPosition>>
    {
    public:
        ActiveQuestGiversValue(PlayerbotAI* ai) : CalculatedValue(ai, "active quest givers", 5) {}

        virtual list<GuidPosition> Calculate();
    };

    //All quest takers that the bot has a quest for.
    class ActiveQuestTakersValue : public CalculatedValue<list<GuidPosition>>
    {
    public:
        ActiveQuestTakersValue(PlayerbotAI* ai) : CalculatedValue(ai, "active quest takers", 5) {}

        virtual list<GuidPosition> Calculate();
    };

    //All objectives that the bot still has to complete.
    class ActiveQuestObjectivesValue : public CalculatedValue<list<GuidPosition>>
    {
    public:
        ActiveQuestObjectivesValue(PlayerbotAI* ai) : CalculatedValue(ai, "active quest objectives", 5) {}

        virtual list<GuidPosition> Calculate();
    };

    //Free quest log slots
    class FreeQuestLogSlotValue : public Uint8CalculatedValue
    {
    public:
        FreeQuestLogSlotValue(PlayerbotAI* ai) : Uint8CalculatedValue(ai, "free quest log slots") {}

        virtual uint8 Calculate();
    };
}
