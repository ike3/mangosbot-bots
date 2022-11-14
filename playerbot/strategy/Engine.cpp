#include "../../botpch.h"
#include "../playerbot.h"
#include <stdarg.h>
#include <iomanip>

#include "Engine.h"
#include "../PlayerbotAIConfig.h"
#include "../PerformanceMonitor.h"

using namespace ai;
using namespace std;

Engine::Engine(PlayerbotAI* ai, AiObjectContext *factory) : PlayerbotAIAware(ai), aiObjectContext(factory)
{
    lastRelevance = 0.0f;
    testMode = false;
}

bool ActionExecutionListeners::Before(Action* action, const Event& event)
{
    bool result = true;
    for (list<ActionExecutionListener*>::iterator i = listeners.begin(); i!=listeners.end(); i++)
    {
        result &= (*i)->Before(action, event);
    }
    return result;
}

void ActionExecutionListeners::After(Action* action, bool executed, const Event& event)
{
    for (list<ActionExecutionListener*>::iterator i = listeners.begin(); i!=listeners.end(); i++)
    {
        (*i)->After(action, executed, event);
    }
}

bool ActionExecutionListeners::OverrideResult(Action* action, bool executed, const Event& event)
{
    bool result = executed;
    for (list<ActionExecutionListener*>::iterator i = listeners.begin(); i!=listeners.end(); i++)
    {
        result = (*i)->OverrideResult(action, result, event);
    }
    return result;
}

bool ActionExecutionListeners::AllowExecution(Action* action, const Event& event)
{
    bool result = true;
    for (list<ActionExecutionListener*>::iterator i = listeners.begin(); i!=listeners.end(); i++)
    {
        result &= (*i)->AllowExecution(action, event);
    }
    return result;
}

ActionExecutionListeners::~ActionExecutionListeners()
{
    for (list<ActionExecutionListener*>::iterator i = listeners.begin(); i!=listeners.end(); i++)
    {
        delete *i;
    }
    listeners.clear();
}


Engine::~Engine(void)
{
    Reset();

    strategies.clear();
}

void Engine::Reset()
{
    ActionNode* action = NULL;
    do
    {
        action = queue.Pop();
        if (!action) break;
        delete action;
    } while (true);

    for (list<TriggerNode*>::iterator i = triggers.begin(); i != triggers.end(); i++)
    {
        TriggerNode* trigger = *i;
        delete trigger;
    }
    triggers.clear();

    for (list<Multiplier*>::iterator i = multipliers.begin(); i != multipliers.end(); i++)
    {
        Multiplier* multiplier = *i;
        delete multiplier;
    }
    multipliers.clear();
}

void Engine::Init()
{
    Reset();

    for (map<string, Strategy*>::iterator i = strategies.begin(); i != strategies.end(); i++)
    {
        Strategy* strategy = i->second;
        strategy->InitMultipliers(multipliers);
        strategy->InitTriggers(triggers);
        Event emptyEvent;
        MultiplyAndPush(strategy->getDefaultActions(), 0.0f, false, emptyEvent, "default");
    }

	if (testMode)
	{
        FILE* file = fopen("test.log", "w");
        fprintf(file, "\n");
        fclose(file);
	}
}


bool Engine::DoNextAction(Unit* unit, int depth, bool minimal)
{
    LogAction("--- AI Tick ---");
    if (sPlayerbotAIConfig.logValuesPerTick)
        LogValues();

    bool actionExecuted = false;
    ActionBasket* basket = NULL;

    time_t currentTime = time(0);
    aiObjectContext->Update();
    ProcessTriggers(minimal);

    int iterations = 0;
    int iterationsPerTick = queue.Size() * (minimal ? (uint32)(sPlayerbotAIConfig.iterationsPerTick / 2) : sPlayerbotAIConfig.iterationsPerTick);
    do {
        basket = queue.Peek();
        if (basket) {
            float relevance = basket->getRelevance(); // just for reference
            bool skipPrerequisites = basket->isSkipPrerequisites();
            Event event = basket->getEvent();
            if (minimal && (relevance < 100))
                continue;
            // NOTE: queue.Pop() deletes basket
            ActionNode* actionNode = queue.Pop();
            Action* action = InitializeAction(actionNode);
            
            PerformanceMonitorOperation* pmo1 = sPerformanceMonitor.start(PERF_MON_ACTION, (event.getSource().empty() ? event.getSource() : (action ? action->getName(): "unknown")), &aiObjectContext->performanceStack);

            if(action)
                action->setRelevance(relevance);

            if (!action)
            {
                if (ai->HasStrategy("debug action", BotState::BOT_STATE_NON_COMBAT))
                {
                    ostringstream out;
                    out << "try: ";
                    out << actionNode->getName();
                    out << " unknown (";

                    out << std::fixed << std::setprecision(3);
                    out << relevance << ")";

                    if (!event.getSource().empty())
                        out << " [" << event.getSource() << "]";

                    ai->TellMasterNoFacing(out);
                }
                LogAction("A:%s - UNKNOWN", actionNode->getName().c_str());
            }
            else
            {
                PerformanceMonitorOperation* pmo2 = sPerformanceMonitor.start(PERF_MON_ACTION, action->getName() + " (usefull)", &aiObjectContext->performanceStack);
                bool isUsefull = action->isUseful();
                if (pmo2) pmo2->finish();

                if (isUsefull)
                {
                    for (list<Multiplier*>::iterator i = multipliers.begin(); i != multipliers.end(); i++)
                    {
                        Multiplier* multiplier = *i;
                        relevance *= multiplier->GetValue(action);
                        action->setRelevance(relevance);
                        if (!relevance)
                        {
                            LogAction("Multiplier %s made action %s useless", multiplier->getName().c_str(), action->getName().c_str());
                            break;
                        }
                    }

                    PerformanceMonitorOperation* pmo3 = sPerformanceMonitor.start(PERF_MON_ACTION, action->getName() + " (possible)", &aiObjectContext->performanceStack);
                    bool isPossible = action->isPossible();
                    if (pmo3) pmo3->finish();

                    if (isPossible && relevance)
                    {
                        if (!skipPrerequisites)
                        {
                            LogAction("A:%s - PREREQ", action->getName().c_str());
                            if (MultiplyAndPush(actionNode->getPrerequisites(), relevance + 0.02, false, event, "prereq"))
                            {
                                PushAgain(actionNode, relevance + 0.01, event);

                                if (pmo1) pmo1->finish();
                                continue;
                            }
                        }

                        PerformanceMonitorOperation* pmo4 = sPerformanceMonitor.start(PERF_MON_ACTION, action->getName() + " (execute)", &aiObjectContext->performanceStack);
                        actionExecuted = ListenAndExecute(action, event);
                        if (pmo4) pmo4->finish();

                        if (actionExecuted)
                        {
                            LogAction("A:%s - OK", action->getName().c_str());
                            MultiplyAndPush(actionNode->getContinuers(), 0, false, event, "cont");
                            lastRelevance = relevance;
                            delete actionNode;
                            if (pmo1) pmo1->finish();
                            break;
                        }
                        else
                        {
                            LogAction("A:%s - FAILED", action->getName().c_str());
                            MultiplyAndPush(actionNode->getAlternatives(), relevance + 0.03, false, event, "alt");
                        }
                    }
                    else
                    {
                        if (ai->HasStrategy("debug action", BotState::BOT_STATE_NON_COMBAT))
                        {
                            ostringstream out;
                            out << "try: ";
                            out << action->getName();
                            out << " impossible (";

                            out << std::fixed << std::setprecision(3);
                            out << action->getRelevance() << ")";

                            if (!event.getSource().empty())
                                out << " [" << event.getSource() << "]";

                            ai->TellMasterNoFacing(out);
                        }
                        LogAction("A:%s - IMPOSSIBLE", action->getName().c_str());
                        MultiplyAndPush(actionNode->getAlternatives(), relevance + 0.03, false, event, "alt");
                    }
                }
                else
                {
                    if (ai->HasStrategy("debug action", BotState::BOT_STATE_NON_COMBAT))
                    {
                        ostringstream out;
                        out << "try: ";
                        out << action->getName();
                        out << " useless (";

                        out << std::fixed << std::setprecision(3);
                        out << action->getRelevance() << ")";

                        if (!event.getSource().empty())
                            out << " [" << event.getSource() << "]";

                        ai->TellMasterNoFacing(out);
                    }
                    lastRelevance = relevance;
                    LogAction("A:%s - USELESS", action->getName().c_str());
                }
            }
            delete actionNode;

            if (pmo1) pmo1->finish();
        }
    }
    while (basket && ++iterations <= iterationsPerTick);

    if (!basket)
    {
        lastRelevance = 0.0f;
        PushDefaultActions();
        if (queue.Peek() && depth < 2)
            return DoNextAction(unit, depth + 1, minimal);
    }

    // MEMORY FIX TEST
 /*   do {
        basket = queue.Peek();
        if (basket) {
            // NOTE: queue.Pop() deletes basket
            delete queue.Pop();
        }
    } while (basket);*/

    if (time(0) - currentTime > 1) {
        LogAction("too long execution");
    }

    if (!actionExecuted)
        LogAction("no actions executed");

    queue.RemoveExpired();
    return actionExecuted;
}

ActionNode* Engine::CreateActionNode(string name)
{
    for (map<string, Strategy*>::iterator i = strategies.begin(); i != strategies.end(); i++)
    {
        Strategy* strategy = i->second;
        ActionNode* node = strategy->GetAction(name);
        if (node)
            return node;
    }
    return new ActionNode (name,
        /*P*/ NULL,
        /*A*/ NULL,
        /*C*/ NULL);
}

bool Engine::MultiplyAndPush(NextAction** actions, float forceRelevance, bool skipPrerequisites, const Event& event, const char* pushType)
{
    bool pushed = false;
    if (actions)
    {
        for (int j=0; actions[j]; j++)
        {
            NextAction* nextAction = actions[j];
            if (nextAction)
            {
                ActionNode* action = CreateActionNode(nextAction->getName());
                InitializeAction(action);

                float k = nextAction->getRelevance();
                if (forceRelevance > 0.0f)
                {
                    k = forceRelevance;
                }

                if (k > 0)
                {
                    LogAction("PUSH:%s - %f (%s)", action->getName().c_str(), k, pushType);
                    queue.Push(new ActionBasket(action, k, skipPrerequisites, event));
                    pushed = true;
                }
                else
                {
                    delete action;
                }

                delete nextAction;
            }
            else
                break;
        }
        delete[] actions;
    }
    return pushed;
}

ActionResult Engine::ExecuteAction(string name, Event& event, string qualifier)
{
    ActionResult actionResult = ACTION_RESULT_UNKNOWN;
    ActionNode* actionNode = CreateActionNode(name);
    if (actionNode)
    {
        Action* action = InitializeAction(actionNode);
        if (action)
        {
            if (!qualifier.empty())
            {
                Qualified* qualified = dynamic_cast<Qualified*>(action);
                if (qualified)
                {
                    qualified->Qualify(qualifier);
                }
            }

            if (action->isPossible())
            {
                if (action->isUseful())
                {
                    action->MakeVerbose();
                    bool executionResult = ListenAndExecute(action, event);
                    MultiplyAndPush(action->getContinuers(), 0.0f, false, event, "default");
                    actionResult = executionResult ? ACTION_RESULT_OK : ACTION_RESULT_FAILED;
                }
                else
                {
                    actionResult = ACTION_RESULT_USELESS;
                }
            }
            else
            {
                actionResult = ACTION_RESULT_IMPOSSIBLE;
            }
        }

        delete actionNode;
    }

    return actionResult;
}

bool Engine::CanExecuteAction(string name, string qualifier, bool isPossible, bool isUseful)
{
    bool result = true;
    ActionNode* actionNode = CreateActionNode(name);
    if (actionNode)
    {
        Action* action = InitializeAction(actionNode);
        if (action)
        {
            if (!qualifier.empty())
            {
                Qualified* qualified = dynamic_cast<Qualified*>(action);
                if (qualified)
                {
                    qualified->Qualify(qualifier);
                }
            }

            if (isPossible)
            {
                result &= action->isPossible();
            }

            if (isUseful)
            {
                result &= action->isUseful();
            }
        }

        delete actionNode;
    }

    return result;
}

void Engine::addStrategy(string name)
{
    removeStrategy(name);

    Strategy* strategy = aiObjectContext->GetStrategy(name);
    if (strategy)
    {
        set<string> siblings = aiObjectContext->GetSiblingStrategy(name);
        for (set<string>::iterator i = siblings.begin(); i != siblings.end(); i++)
            removeStrategy(*i);

        LogAction("S:+%s", strategy->getName().c_str());
        strategies[strategy->getName()] = strategy;
    }
    if(!initMode)
        Init();
}

void Engine::addStrategies(string first, ...)
{
	addStrategy(first);

	va_list vl;
	va_start(vl, first);

	const char* cur;
	do
	{
		cur = va_arg(vl, const char*);
		if (cur)
			addStrategy(cur);
	}
	while (cur);

	va_end(vl);
}

bool Engine::removeStrategy(string name)
{
    map<string, Strategy*>::iterator i = strategies.find(name);
    if (i == strategies.end())
        return false;

    LogAction("S:-%s", name.c_str());
    strategies.erase(i);
    Init();
    return true;
}

void Engine::removeAllStrategies()
{
    strategies.clear();
    Init();
}

void Engine::toggleStrategy(string name)
{
    if (!removeStrategy(name))
        addStrategy(name);
}

bool Engine::HasStrategy(string name)
{
    return strategies.find(name) != strategies.end();
}

Strategy* Engine::GetStrategy(string name) const
{
    auto i = strategies.find(name);
    if (i != strategies.end())
    {
        return i->second;
    }

    return nullptr;
}

void Engine::ProcessTriggers(bool minimal)
{
    map<Trigger*, Event> fires;
    for (list<TriggerNode*>::iterator i = triggers.begin(); i != triggers.end(); i++)
    {
        TriggerNode* node = *i;
        if (!node)
            continue;

        Trigger* trigger = node->getTrigger();
        if (!trigger)
        {
            trigger = aiObjectContext->GetTrigger(node->getName());
            node->setTrigger(trigger);
        }

        if (!trigger)
            continue;

        if (testMode || trigger->needCheck())
        {
            if (minimal && node->getFirstRelevance() < 100)
                continue;

            PerformanceMonitorOperation* pmo = sPerformanceMonitor.start(PERF_MON_TRIGGER, trigger->getName(), &aiObjectContext->performanceStack);
            Event event = trigger->Check();
            if (pmo) pmo->finish();
            if (!event)
                continue;
            fires[trigger] = event;
            LogAction("T:%s", trigger->getName().c_str());
        }
    }

    for (list<TriggerNode*>::iterator i = triggers.begin(); i != triggers.end(); i++)
    {
        TriggerNode* node = *i;
        Trigger* trigger = node->getTrigger();
        Event& event = fires[trigger];
        if (!event)
            continue;

        MultiplyAndPush(node->getHandlers(), 0.0f, false, event, "trigger");
    }

    for (list<TriggerNode*>::iterator i = triggers.begin(); i != triggers.end(); i++)
    {
        Trigger* trigger = (*i)->getTrigger();
        if (trigger) trigger->Reset();
    }
}

void Engine::PushDefaultActions()
{
    for (map<string, Strategy*>::iterator i = strategies.begin(); i != strategies.end(); i++)
    {
        Strategy* strategy = i->second;
        Event emptyEvent;
        MultiplyAndPush(strategy->getDefaultActions(), 0.0f, false, emptyEvent, "default");
    }
}

string Engine::ListStrategies()
{   
    string s;
    if (strategies.empty())
        return s;

    for (map<string, Strategy*>::iterator i = strategies.begin(); i != strategies.end(); i++)
    {
        s.append(i->first);
        s.append(", ");
    }
    return s.substr(0, s.length() - 2);
}

list<string> Engine::GetStrategies()
{
    list<string> result;
    for (map<string, Strategy*>::iterator i = strategies.begin(); i != strategies.end(); i++)
    {
        result.push_back(i->first);
    }
    return result;
}

void Engine::PushAgain(ActionNode* actionNode, float relevance, const Event& event)
{
    NextAction** nextAction = new NextAction*[2];
    nextAction[0] = new NextAction(actionNode->getName(), relevance);
    nextAction[1] = NULL;
    MultiplyAndPush(nextAction, relevance, true, event, "again");
    delete actionNode;
}

bool Engine::ContainsStrategy(StrategyType type)
{
	for (map<string, Strategy*>::iterator i = strategies.begin(); i != strategies.end(); i++)
	{
		Strategy* strategy = i->second;
		if (strategy->GetType() & type)
			return true;
	}
	return false;
}

Action* Engine::InitializeAction(ActionNode* actionNode)
{
    Action* action = actionNode->getAction();
    if (!action)
    {
        action = aiObjectContext->GetAction(actionNode->getName());
        actionNode->setAction(action);
    }

    if (action != nullptr)
    {
        action->SetReaction(false);
    }

    return action;
}

bool Engine::ListenAndExecute(Action* action, Event& event)
{
    bool actionExecuted = false;
    if (actionExecutionListeners.Before(action, event))
    {
        actionExecuted = actionExecutionListeners.AllowExecution(action, event) ? action->Execute(event) : true;
    }

    if (ai->HasStrategy("debug", BotState::BOT_STATE_NON_COMBAT))
    {
        ostringstream out;
        out << "do: ";
        out << action->getName();
        if (actionExecuted)
            out << " 1 (";
        else
            out << " 0 (";

        out << std::fixed << std::setprecision(2);
        out << action->getRelevance() << ")";

        if(!event.getSource().empty())
            out << " [" << event.getSource() << "]";

        if (actionExecuted)
        {
            const uint32 actionDuration = ai->GetAIInternalUpdateDelay();
            if (actionDuration > 0)
            {
                out << " (duration: " << ((float)actionDuration / IN_MILLISECONDS) << "s)";
            }
        }

        ai->TellMasterNoFacing(out);
    }

    actionExecuted = actionExecutionListeners.OverrideResult(action, actionExecuted, event);
    actionExecutionListeners.After(action, actionExecuted, event);
    return actionExecuted;
}

void Engine::LogAction(const char* format, ...)
{
    char buf[1024];

    va_list ap;
    va_start(ap, format);
    vsprintf(buf, format, ap);
    va_end(ap);
    lastAction += "|";
    lastAction += buf;
    if (lastAction.size() > 512)
    {
        lastAction = lastAction.substr(512);
        size_t pos = lastAction.find("|");
        lastAction = (pos == string::npos ? "" : lastAction.substr(pos));
    }

    if (testMode)
    {
        FILE* file = fopen("test.log", "a");
        fprintf(file, "%s",buf);
        fprintf(file, "\n");
        fclose(file);
    }
    else
    {
        Player* bot = ai->GetBot();
        if (sPlayerbotAIConfig.logInGroupOnly && !bot->GetGroup())
            return;

        sLog.outDetail( "%s %s", bot->GetName(), buf);
    }
}

void Engine::ChangeStrategy(string names, string engineType)
{
    vector<string> splitted = split(names, ',');
    for (vector<string>::iterator i = splitted.begin(); i != splitted.end(); i++)
    {
        const char* name = i->c_str();
        switch (name[0])
        {
            case '+':
            {
                addStrategy(name+1);
                break;
            }
            case '-':
            {
                removeStrategy(name+1);
                break;
            }
            case '~':
            {
                toggleStrategy(name+1);
                break;
            }
            case '?':
            {
                string engineStrategies = engineType;
                engineStrategies.append(" Strategies: ");
                engineStrategies.append(ListStrategies());
                ai->TellMaster(engineStrategies);
                break;
            }
        }
    }
}

void Engine::LogValues()
{
    if (testMode)
        return;

    Player* bot = ai->GetBot();
    if (sPlayerbotAIConfig.logInGroupOnly && !bot->GetGroup())
        return;

    string text = ai->GetAiObjectContext()->FormatValues();
    sLog.outDebug( "Values for %s: %s", bot->GetName(), text.c_str());
}