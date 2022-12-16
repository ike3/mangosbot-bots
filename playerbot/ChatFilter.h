#pragma once

using namespace std;

namespace ai
{
    class ChatFilter : public PlayerbotAIAware
    {
    public:
        ChatFilter(PlayerbotAI* ai) : PlayerbotAIAware(ai) {}
        virtual string Filter(string message) { return Filter(message, ""); }
        virtual string Filter(string message, string filter);
		virtual ~ChatFilter() {}

#ifdef GenerateBotHelp
        virtual string GetHelpName() { return "dummy"; }
        virtual unordered_map<string, string> GetFilterExamples() {return {};}
        virtual string GetHelpDescription() { return ""; }
#endif
    };

    class CompositeChatFilter : public ChatFilter
    {
    public:
        CompositeChatFilter(PlayerbotAI* ai);
        virtual ~CompositeChatFilter();
        string Filter(string message);
#ifdef GenerateBotHelp
        virtual list<ChatFilter*> GetFilters() { return filters;}
#endif
    private:
        list<ChatFilter*> filters;
    };
};
