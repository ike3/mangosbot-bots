#pragma once
#include "../Value.h"
#include "LootValues.h"

namespace ai
{          

    class QuestObjectMgr : ObjectMgr
    {
    public:
        QuestRelationsMap& GetCreatureQuestRelationsMap() { return m_CreatureQuestRelations; }
        QuestRelationsMap& GetCreatureQuestInvolvedRelationsMap() { return m_CreatureQuestInvolvedRelations; }
        QuestRelationsMap& GetGOQuestRelationsMap() { return m_GOQuestRelations; }
        QuestRelationsMap& GetGOQuestInvolvedRelationsMap() { return m_GOQuestInvolvedRelations; }
    };

    enum class QuestRelationFlag : uint32
    {
        none = 0,
        objective1 = 1,
        objective2 = 2,
        objective3 = 4,
        objective4 = 8,
        questGiver = 16,
        questTaker = 32,
        maxQuestRelationFlag = 64
    };   


    //                     questId, QuestRelationFlag
    typedef unordered_map <uint32, uint32> questRelationMap;
    //                     entry
    typedef unordered_map <int32, questRelationMap> entryQuestRelationMap;

    //                      entry
    typedef unordered_map < int32, list<GuidPosition>> questEntryGuidps;
    //                      QuestRelationFlag
    typedef unordered_map < uint32, questEntryGuidps> questRelationGuidps;

    //                      questId
    typedef unordered_map < uint32, questRelationGuidps> questGuidpMap;
    

    //                      questId
    typedef unordered_map < uint32, list<GuidPosition>> questGiverMap;
    
    
    //Returns the quest relation Flags for all entries and quests
    class EntryQuestRelationMapValue : public SingleCalculatedValue<entryQuestRelationMap>
    {
    public:
        EntryQuestRelationMapValue(PlayerbotAI* ai) : SingleCalculatedValue(ai, "entry quest relation map") {}

        virtual entryQuestRelationMap Calculate();
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
        unordered_map<int32, vector<pair<uint32, QuestRelationFlag>>> entryMap;
        unordered_map<uint32, vector<pair<uint32, QuestRelationFlag>>> itemMap;

        entryQuestRelationMap relationMap;

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
    class QuestGiversValue : public SingleCalculatedValue<questGiverMap>, public Qualified
	{
	public:
        QuestGiversValue(PlayerbotAI* ai) : SingleCalculatedValue(ai, "quest givers") {}

        virtual questGiverMap Calculate();
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
        FreeQuestLogSlotValue(PlayerbotAI* ai) : Uint8CalculatedValue(ai, "free quest log slots", 2) {}

        virtual uint8 Calculate();
    };
    
    //Dialog status npc 
    class DialogStatusValue : public Uint32CalculatedValue, public Qualified
    {
    public:
        DialogStatusValue(PlayerbotAI* ai, string name = "dialog status") : Uint32CalculatedValue(ai, name , 2) {}

        static uint32 getDialogStatus(Player* bot, int32 questgiver, uint32 questId = 0);

        virtual uint32 Calculate() { return getDialogStatus(bot, stoi(getQualifier())); }
    };

    //Dialog status npc quest
    class DialogStatusQuestValue : public DialogStatusValue
    {
    public:
        DialogStatusQuestValue(PlayerbotAI* ai) : DialogStatusValue(ai, "dialog status quest") {}

        virtual uint32 Calculate() { return getDialogStatus(bot, getMultiQualifier(getQualifier(), 0), getMultiQualifier(getQualifier(), 1)); }
    };

    //Can accept quest from npc
    class CanAcceptQuestValue : public BoolCalculatedValue, public Qualified
    {
    public:
        CanAcceptQuestValue(PlayerbotAI* ai) : BoolCalculatedValue(ai, "can accept quest npc") {}

        virtual bool Calculate() { return AI_VALUE2(uint32, "dialog status", getQualifier()) == DIALOG_STATUS_AVAILABLE;};
    };

    //Can accept low level quest from npc
    class CanAcceptQuestLowLevelValue : public BoolCalculatedValue, public Qualified
    {
    public:
        CanAcceptQuestLowLevelValue(PlayerbotAI* ai) : BoolCalculatedValue(ai, "can accept quest low level npc") {}
        virtual bool Calculate()
        {
            uint32 dialogStatus = AI_VALUE2(uint32, "dialog status", getQualifier());
#ifndef MANGOSBOT_TWO
            return dialogStatus == DIALOG_STATUS_CHAT;
#else
            return dialogStatus == DIALOG_STATUS_LOW_LEVEL_AVAILABLE;
#endif   
        };
    };

    //Can hand in quest to npc
    class CanTurnInQuestValue : public BoolCalculatedValue, public Qualified
    {
    public:
        CanTurnInQuestValue(PlayerbotAI* ai) : BoolCalculatedValue(ai, "can turn in quest npc") {}

        virtual bool Calculate()
        {
            uint32 dialogStatus = AI_VALUE2(uint32, "dialog status", getQualifier());
#ifdef MANGOSBOT_ZERO  
            return dialogStatus == DIALOG_STATUS_REWARD2 || dialogStatus == DIALOG_STATUS_REWARD_REP;
#else
            return dialogStatus == DIALOG_STATUS_REWARD2 || dialogStatus == DIALOG_STATUS_REWARD || dialogStatus == DIALOG_STATUS_REWARD_REP;
#endif  
        };
    };
}
