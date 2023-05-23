#pragma once
#include <string>

class PlayerbotAI;

using namespace std;

namespace ai
{
    class PlayerbotAIAware 
    {
    public:
        PlayerbotAIAware(PlayerbotAI* const ai) : ai(ai) { }
        virtual ~PlayerbotAIAware() = default;
        virtual string getName() { return string(); }
    protected:
        PlayerbotAI* ai;
    };
}