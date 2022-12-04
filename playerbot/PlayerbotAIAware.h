#pragma once

class PlayerbotAI;

namespace ai
{
    class PlayerbotAIAware 
    {
    public:
        PlayerbotAIAware(PlayerbotAI* const ai) : ai(ai) { }
        virtual std::string getName() { return std::string(); }
    protected:
        PlayerbotAI* ai;
    };
}