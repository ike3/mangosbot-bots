#pragma once
#include "Event.h"
#include "Value.h"
#include "AiObject.h"

class Unit;

namespace ai
{
    class NextAction;
    template<class T> class Value;
}

namespace ai
{
    class Trigger : public AiNamedObject
	{
	public:
        Trigger(PlayerbotAI* ai, string name = "trigger", int checkInterval = 1) : AiNamedObject(ai, name), triggered(false) {
			this->checkInterval = checkInterval;
			lastCheckTime = time(0) - rand() % checkInterval;
		}
        virtual ~Trigger() {}

	public:
        virtual Event Check();
        virtual void ExternalEvent(string param, Player* owner = NULL) {}
        virtual void ExternalEvent(WorldPacket &packet, Player* owner = NULL) {}
        virtual void ExternalEventForce(string param, Player* owner = NULL)
        {
            this->param = param;
            this->owner = owner;
            triggered = true;
        }
        virtual bool IsActive() { return false; }
        virtual NextAction** getHandlers() { return NULL; }
        void Update() {}
        virtual void Reset() { triggered = false; }
        virtual Unit* GetTarget();
        virtual Value<Unit*>* GetTargetValue();
        virtual string GetTargetName() { return "self target"; }

		bool needCheck() {
		    if (checkInterval < 2) return true;

		    time_t now = time(0);
			if (!lastCheckTime || now - lastCheckTime >= checkInterval) {
			    lastCheckTime = now;
				return true;
			}
			return false;
		}

#ifndef GenerateBotHelp
        virtual string GetHelpName() { return "dummy"; } //Must equal iternal name
        virtual string GetHelpDescription() { return "This is a trigger."; }
        virtual vector<string> GetUsedTriggers() { return {}; }
        virtual vector<string> GetUsedValues() { return {}; }
#endif

    protected:
		int checkInterval;
		time_t lastCheckTime;
        string param;
        bool triggered;
        Player* owner;
	};


    class TriggerNode
    {
    public:
        TriggerNode(string name, NextAction** handlers = NULL)
        {
            this->name = name;
            this->handlers = handlers;
            this->trigger = NULL;
        }
        virtual ~TriggerNode();

    public:
        Trigger* getTrigger() { return trigger; }
        void setTrigger(Trigger* trigger) { this->trigger = trigger; }
        string getName() { return name; }

    public:
        NextAction** getHandlers();
        float getFirstRelevance();
    private:
        std::string name;
        Trigger* trigger;
        NextAction** handlers;
    };
}
