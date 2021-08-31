#pragma once
#include "Action.h"
#include "Event.h"
#include "../PlayerbotAIAware.h"
#include "../PerformanceMonitor.h"
#include "AiObject.h"

namespace ai
{
    class UntypedValue : public AiNamedObject
    {
    public:
        UntypedValue(PlayerbotAI* ai, string name) : AiNamedObject(ai, name) {}
        virtual void Update() {}
        virtual void Reset() {}
        virtual string Format() { return "?"; }
        virtual string Save() { return "?"; }
        virtual bool Load(string value) { return false; }
    };

    template<class T>
    class Value
    {
    public:
        virtual T Get() = 0;
        virtual void Set(T value) = 0;
        operator T() { return Get(); }
    };

    template <class T>
    class SingleCalculatedValue : public UntypedValue, public Value<T>
    {
    public:
        SingleCalculatedValue(PlayerbotAI* ai, string name = "value") : UntypedValue(ai, name) { Reset(); }

        virtual ~SingleCalculatedValue() {}

        virtual T Get()
        {
            if (!calculated)
            {
                value = Calculate();
                calculated = true;
            }
            return value;
        }

        virtual void Set(T value) { this->value = value; }
        virtual void Update() { }

        virtual void Reset()
        {
            calculated = false;
        }
    protected:
        virtual T Calculate() = 0;
    protected:
        T value;
        bool calculated;
    };

    template<class T>
    class CalculatedValue : public UntypedValue, public Value<T>
	{
	public:
        CalculatedValue(PlayerbotAI* ai, string name = "value", int checkInterval = 1) : UntypedValue(ai, name),
            checkInterval(checkInterval)
        {
            lastCheckTime = time(0) - checkInterval;
        }
        virtual ~CalculatedValue() {}

	public:
        virtual T Get()
        {
            time_t now = time(0);
            if (!lastCheckTime || checkInterval < 2 || now - lastCheckTime >= checkInterval / 2)
            {
                lastCheckTime = now;

                PerformanceMonitorOperation *pmo = sPerformanceMonitor.start(PERF_MON_VALUE, getName());
                value = Calculate();
                if (pmo) pmo->finish();
            }
            return value;
        }
        virtual void Set(T value) { this->value = value; }
        virtual void Update() { }
        virtual void Reset() { lastCheckTime = 0; }
    protected:
        virtual T Calculate() = 0;

    protected:
		int checkInterval;
		time_t lastCheckTime;
        T value;
	};

    class Uint8CalculatedValue : public CalculatedValue<uint8>
    {
    public:
        Uint8CalculatedValue(PlayerbotAI* ai, string name = "value", int checkInterval = 1) :
            CalculatedValue<uint8>(ai, name, checkInterval) {}

        virtual string Format()
        {
            ostringstream out; out << (int)Calculate();
            return out.str();
        }
    };

    class Uint32CalculatedValue : public CalculatedValue<uint32>
    {
    public:
        Uint32CalculatedValue(PlayerbotAI* ai, string name = "value", int checkInterval = 1) :
            CalculatedValue<uint32>(ai, name, checkInterval) {}

        virtual string Format()
        {
            ostringstream out; out << (int)Calculate();
            return out.str();
        }
    };

    class FloatCalculatedValue : public CalculatedValue<float>
    {
    public:
        FloatCalculatedValue(PlayerbotAI* ai, string name = "value", int checkInterval = 1) :
            CalculatedValue<float>(ai, name, checkInterval) {}

        virtual string Format()
        {
            ostringstream out; out << Calculate();
            return out.str();
        }
    };

    class BoolCalculatedValue : public CalculatedValue<bool>
    {
    public:
        BoolCalculatedValue(PlayerbotAI* ai, string name = "value", int checkInterval = 1) :
            CalculatedValue<bool>(ai, name, checkInterval) {}

        virtual string Format()
        {
            return Calculate() ? "true" : "false";
        }
    };

    class UnitCalculatedValue : public CalculatedValue<Unit*>
    {
    public:
        UnitCalculatedValue(PlayerbotAI* ai, string name = "value", int checkInterval = 1) :
            CalculatedValue<Unit*>(ai, name, checkInterval) { lastCheckTime = time(0) - checkInterval / 2; }

        virtual string Format()
        {
            Unit* unit = Calculate();
            return unit ? unit->GetName() : "<none>";
        }
    };
    
    class CDPairCalculatedValue : public CalculatedValue<CreatureDataPair const*>
    {
    public:
        CDPairCalculatedValue(PlayerbotAI* ai, string name = "value", int checkInterval = 1) :
            CalculatedValue<CreatureDataPair const*>(ai, name, checkInterval) { lastCheckTime = time(0) - checkInterval / 2; }

        virtual string Format()
        {
            CreatureDataPair const* creatureDataPair = Calculate();
            CreatureInfo const* bmTemplate = ObjectMgr::GetCreatureTemplate(creatureDataPair->second.id);
            return creatureDataPair ? bmTemplate->Name : "<none>";
        }
    };

    class CDPairListCalculatedValue : public CalculatedValue<list<CreatureDataPair const*>>
    {
    public:
        CDPairListCalculatedValue(PlayerbotAI* ai, string name = "value", int checkInterval = 1) :
            CalculatedValue<list<CreatureDataPair const*>>(ai, name, checkInterval) { lastCheckTime = time(0) - checkInterval / 2; }

        virtual string Format()
        {
            ostringstream out; out << "{";
            list<CreatureDataPair const*> cdPairs = Calculate();
            for (list<CreatureDataPair const*>::iterator i = cdPairs.begin(); i != cdPairs.end(); ++i)
            {
                CreatureDataPair const* cdPair = *i;
                out << cdPair->first << ",";
            }
            out << "}";
            return out.str();
        }
    };

    class ObjectGuidCalculatedValue : public CalculatedValue<ObjectGuid>
    {
    public:
        ObjectGuidCalculatedValue(PlayerbotAI* ai, string name = "value", int checkInterval = 1) :
            CalculatedValue<ObjectGuid>(ai, name, checkInterval) { lastCheckTime = time(0) - checkInterval / 2; }

        virtual string Format()
        {
            ObjectGuid guid = Calculate();
            return guid ? to_string(guid.GetRawValue()) : "<none>";
        }
    };

    class ObjectGuidListCalculatedValue : public CalculatedValue<list<ObjectGuid> >
    {
    public:
        ObjectGuidListCalculatedValue(PlayerbotAI* ai, string name = "value", int checkInterval = 1) :
            CalculatedValue<list<ObjectGuid> >(ai, name, checkInterval) { lastCheckTime = time(0) - checkInterval/2; }

        virtual string Format()
        {
            ostringstream out; out << "{";
            list<ObjectGuid> guids = Calculate();
            for (list<ObjectGuid>::iterator i = guids.begin(); i != guids.end(); ++i)
            {
                ObjectGuid guid = *i;
                out << guid.GetRawValue() << ",";
            }
            out << "}";
            return out.str();
        }
    };

    template<class T>
    class ManualSetValue : public UntypedValue, public Value<T>
    {
    public:
        ManualSetValue(PlayerbotAI* ai, T defaultValue, string name = "value") :
            UntypedValue(ai, name), value(defaultValue), defaultValue(defaultValue) {}
        virtual ~ManualSetValue() {}

    public:
        virtual T Get() { return value; }
        virtual void Set(T value) { this->value = value; }
        virtual void Update() { }
        virtual void Reset() { value = defaultValue; }

    protected:
        T value;
        T defaultValue;
    };

    class UnitManualSetValue : public ManualSetValue<Unit*>
    {
    public:
        UnitManualSetValue(PlayerbotAI* ai, Unit* defaultValue, string name = "value") :
            ManualSetValue<Unit*>(ai, defaultValue, name) {}

        virtual string Format()
        {
            Unit* unit = Get();
            return unit ? unit->GetName() : "<none>";
        }
    };
}
