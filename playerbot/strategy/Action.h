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
    class NextAction
    {
    public:
        NextAction(string name, float relevance = 0.0f)
        {
            this->name = name;
            this->relevance = relevance;
        }
        NextAction(const NextAction& o)
        {
            this->name = o.name;
            this->relevance = o.relevance;
        }

    public:
        string getName() { return name; }
        float getRelevance() {return relevance;}

    public:
        static int size(NextAction** actions);
        static NextAction** clone(NextAction** actions);
        static NextAction** merge(NextAction** what, NextAction** with);
        static NextAction** array(uint32 nil,...);
        static void destroy(NextAction** actions);

    private:
        float relevance;
        std::string name;
    };

    //---------------------------------------------------------------------------------------------------------------------

    class ActionBasket;

    enum class ActionThreatType : uint8
    {
        ACTION_THREAT_NONE = 0,
        ACTION_THREAT_SINGLE= 1,
        ACTION_THREAT_AOE = 2
    };

    class Action : public AiNamedObject
	{
	public:
        Action(PlayerbotAI* ai, string name = "action") : verbose(false), AiNamedObject(ai, name) { }
        virtual ~Action(void) {}

    public:
        virtual bool Execute(Event& event) { return true; }
        virtual bool isPossible() { return true; }
        virtual bool isUseful() { return true; }
        virtual NextAction** getPrerequisites() { return NULL; }
        virtual NextAction** getAlternatives() { return NULL; }
        virtual NextAction** getContinuers() { return NULL; }
        virtual ActionThreatType getThreatType() { return ActionThreatType::ACTION_THREAT_NONE; }
        void Update() {}
        void Reset() {}
        virtual Unit* GetTarget();
        virtual Value<Unit*>* GetTargetValue();
        virtual string GetTargetName() { return "self target"; }
        void MakeVerbose() { verbose = true; }

        void setRelevance(float relevance1) { relevance = relevance1; };
        virtual float getRelevance() { return relevance; }

        bool IsReaction() const { return reaction; }
        void SetReaction(bool inReaction) { reaction = inReaction; }

#ifndef GenerateBotHelp
        virtual string GetHelpName() { return "dummy"; } //Must equal iternal name
        virtual string GetHelpDescription() { return "This is an action."; }
        virtual vector<string> GetUsedActions() { return {}; }
        virtual vector<string> GetUsedValues() { return {}; }
#endif        
    protected:
        void SetDuration(uint32 delay);

    protected:
        bool verbose;
        float relevance = 0;
        bool reaction = false;
	};

    class ActionNode
    {
    public:
        ActionNode(string name, NextAction** prerequisites = NULL, NextAction** alternatives = NULL, NextAction** continuers = NULL)
        {
            this->action = NULL;
            this->name = name;
            this->prerequisites = prerequisites;
            this->alternatives = alternatives;
            this->continuers = continuers;
        }
        virtual ~ActionNode()
        {
            NextAction::destroy(prerequisites);
            NextAction::destroy(alternatives);
            NextAction::destroy(continuers);
        }

    public:
        Action* getAction() { return action; }
        void setAction(Action* action) { this->action = action; }
        string getName() { return name; }

    public:
        NextAction** getContinuers() { return NextAction::merge(NextAction::clone(continuers), action->getContinuers()); }
        NextAction** getAlternatives() { return NextAction::merge(NextAction::clone(alternatives), action->getAlternatives()); }
        NextAction** getPrerequisites() { return NextAction::merge(NextAction::clone(prerequisites), action->getPrerequisites()); }

    private:
        string name;
        Action* action;
        NextAction** continuers;
        NextAction** alternatives;
        NextAction** prerequisites;
    };

    //---------------------------------------------------------------------------------------------------------------------

	class ActionBasket
	{
	public:
        ActionBasket(ActionNode* action, float relevance, bool skipPrerequisites, const Event& event) :
          action(action), relevance(relevance), skipPrerequisites(skipPrerequisites), event(event) {
            created = time(0);
        }
        virtual ~ActionBasket(void) {}
	public:
		float getRelevance() {return relevance;}
		ActionNode* getAction() {return action;}
        Event getEvent() { return event; }
        bool isSkipPrerequisites() { return skipPrerequisites; }
        void AmendRelevance(float k) {relevance *= k; }
        void setRelevance(float relevance) { this->relevance = relevance; }
        bool isExpired(time_t secs) { return time(0) - created >= secs; }
	private:
		ActionNode* action;
		float relevance;
        bool skipPrerequisites;
        Event event;
        time_t created;
	};

    //---------------------------------------------------------------------------------------------------------------------


}

#define AI_VALUE(type, name) context->GetValue<type>(name)->Get()
#define AI_VALUE2(type, name, param) context->GetValue<type>(name, param)->Get()

#define AI_VALUE_LAZY(type, name) context->GetValue<type>(name)->LazyGet()
#define AI_VALUE2_LAZY(type, name, param) context->GetValue<type>(name, param)->LazyGet()

#define SET_AI_VALUE(type, name, value) context->GetValue<type>(name)->Set(value)
#define SET_AI_VALUE2(type, name, param, value) context->GetValue<type>(name, param)->Set(value)
#define RESET_AI_VALUE(type, name) context->GetValue<type>(name)->Reset()
#define RESET_AI_VALUE2(type, name, param) context->GetValue<type>(name, param)->Reset()

#define PAI_VALUE(type, name) player->GetPlayerbotAI()->GetAiObjectContext()->GetValue<type>(name)->Get()
#define PAI_VALUE2(type, name, param) player->GetPlayerbotAI()->GetAiObjectContext()->GetValue<type>(name, param)->Get()
#define GAI_VALUE(type, name) sSharedValueContext.getGlobalValue<type>(name)->Get()
#define GAI_VALUE2(type, name, param) sSharedValueContext.getGlobalValue<type>(name, param)->Get()

#define MEM_AI_VALUE(type, name) dynamic_cast<MemoryCalculatedValue<type>*>(context->GetUntypedValue(name))
#define LOG_AI_VALUE(type, name) dynamic_cast<LogCalculatedValue<type>*>(context->GetUntypedValue(name))