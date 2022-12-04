#ifndef _PlayerbotHelpMgr_H
#define _PlayerbotHelpMgr_H

#include "Common.h"
#include "PlayerbotAIBase.h"

#define BOT_HELP(name) sPlayerbotHelpMgr.GetBotText(name)

#ifndef GenerateBotHelp
//#define GenerateBotHelp //Enable only for help generation
#endif

using namespace std;

struct BotHelpEntry
{
    BotHelpEntry() { m_new = true; }
    BotHelpEntry(std::string templateText, std::string text, std::map<int32, std::string> text_locales) : m_templateText(templateText), m_text(text), m_text_locales(text_locales) {}
    std::string m_templateText;
    bool m_new = false;
    bool m_templateChanged = false;
    std::string m_text;
    std::map<int32, std::string> m_text_locales;
};

class PlayerbotHelpMgr
{
    public:
        PlayerbotHelpMgr();
        virtual ~PlayerbotHelpMgr();
        static PlayerbotHelpMgr& instance()
        {
            static PlayerbotHelpMgr instance;
            return instance;
        }

	public:
#ifdef GenerateBotHelp
        PlayerbotAI* ai;
        AiObjectContext* genericContext;
        typedef unordered_map<Action*, float> actionMap;
        typedef unordered_map<Trigger*, actionMap> triggerMap;
        typedef unordered_map<BotState, triggerMap> stateMap;
        typedef unordered_map<Strategy*, stateMap> strategyMap;
        unordered_map<string, strategyMap> classMap;
        unordered_map<string, AiObjectContext*> classContext;

        unordered_map<BotState, string> states = { {BotState::BOT_STATE_COMBAT, "combat"},  {BotState::BOT_STATE_NON_COMBAT, "non combat"},  {BotState::BOT_STATE_DEAD, "dead state"},  {BotState::BOT_STATE_REACTION, "reaction"},  {BotState::BOT_STATE_ALL, "all"} };

        unordered_map<string, string> supportedActionName;

        typedef unordered_map<string, bool> nameCoverageMap;
        unordered_map<string, nameCoverageMap> coverageMap;

        static string initcap(string st) { string s = st; s[0] = toupper(s[0]); return s; }
        static string formatFloat(float num);
        static void replace(string& text, const string what, const string with);
        static string makeList(vector<string>const parts, string partFormat = "<part>", uint32 maxLength = 254);

        bool IsGenericSupported(PlayerbotAIAware* object);
        string GetObjectName(PlayerbotAIAware* object, string className);
        string GetObjectLink(PlayerbotAIAware* object, string className);

        void LoadStrategies(string className, AiObjectContext* context);
        void LoadAllStrategies();

        string GetStrategyBehaviour(string className, Strategy* strategy);
        void GenerateStrategyHelp();

        string GetTriggerBehaviour(string className, Trigger* trigger);
        void GenerateTriggerHelp();

        string GetActionBehaviour(string className, Action* Action);
        void GenerateActionHelp();

        void GenerateValueHelp();

        void GenerateChatFilterHelp();

        void PrintCoverage();

        void SaveTemplates();
        
        void GenerateHelp();
#endif
        static void FormatHelpTopic(string& text);
        void FormatHelpTopics();
        void LoadBotHelpTexts();

        string GetBotText(string name);
        bool GetBotText(string name, string& text);

    private:
        unordered_map<string, BotHelpEntry> botHelpText;
};

#define sPlayerbotHelpMgr PlayerbotHelpMgr::instance()

#endif
