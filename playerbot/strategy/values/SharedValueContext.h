#pragma once

#include "PvpValues.h"
#include "QuestValues.h"

namespace ai
{
    class SharedValueContext : public NamedObjectContext<UntypedValue>
    {
    public:
        SharedValueContext() : NamedObjectContext(true)
        {
            creators["bg masters"] = &SharedValueContext::bg_masters;

            creators["drop map"] = &SharedValueContext::drop_map;
            creators["item drop list"] = &SharedValueContext::item_drop_list;
            creators["entry loot list"] = &SharedValueContext::entry_loot_list;
            creators["loot chance"] = &SharedValueContext::loot_chance;

            creators["entry quest relation"] = &SharedValueContext::entry_quest_relation;

            creators["quest guidp map"] = &SharedValueContext::quest_guidp_map;
            creators["quest givers"] = &SharedValueContext::quest_givers;
        }


    private:
        static UntypedValue* bg_masters(PlayerbotAI* ai) { return new BgMastersValue(ai); }

        static UntypedValue* drop_map(PlayerbotAI* ai) { return new DropMapValue(ai); }
        static UntypedValue* item_drop_list(PlayerbotAI* ai) { return new ItemDropListValue(ai); }
        static UntypedValue* entry_loot_list(PlayerbotAI* ai) { return new EntryLootListValue(ai); }
        static UntypedValue* loot_chance(PlayerbotAI* ai) { return new LootChanceValue(ai); }

        static UntypedValue* entry_quest_relation(PlayerbotAI* ai) { return new EntryQuestRelationMapValue(ai); }        

        static UntypedValue* quest_guidp_map(PlayerbotAI* ai) { return new QuestGuidpMapValue(ai); }
        static UntypedValue* quest_givers(PlayerbotAI* ai) { return new QuestGiversValue(ai); }

    //Global acess functions
    public:
        template<class T>
        Value<T>* getGlobalValue(string name = "")
        {
            NamedObjectContextList<UntypedValue> valueContexts;
            valueContexts.Add(this);
            PlayerbotAI* ai = new PlayerbotAI();
            UntypedValue* value = valueContexts.GetObject(name, ai);
            delete ai;
            return dynamic_cast<Value<T>*>(value);
        }

        template<class T>
        Value<T>* getGlobalValue(string name, string param)
        {
            return getGlobalValue<T>((string(name) + "::" + param));
        }

        template<class T>
        Value<T>* getGlobalValue(string name, uint32 param)
        {
            ostringstream out; out << param;
            return getGlobalValue<T>(name, out.str());
        }
    };

#define sSharedValueContext MaNGOS::Singleton<SharedValueContext>::Instance()
}