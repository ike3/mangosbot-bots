#pragma once

class PlayerbotAI;

namespace ai
{
    class PlayerbotAIAware 
    {
    public:
        PlayerbotAIAware(PlayerbotAI* const ai) : ai(ai) { }

    protected:
        PlayerbotAI* ai;
    };
}