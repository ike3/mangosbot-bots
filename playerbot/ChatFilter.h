#pragma once

using namespace std;

namespace ai
{
    class ChatFilter : public PlayerbotAIAware
    {
    public:
        ChatFilter(PlayerbotAI* ai) : PlayerbotAIAware(ai) {}
        virtual string Filter(string message);
		virtual ~ChatFilter() {}

#ifndef GenerateBotHelp
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
#ifndef GenerateBotHelp
        virtual list<ChatFilter*> GetFilters() { return filters;}
#endif
    private:
        list<ChatFilter*> filters;
    };
};
