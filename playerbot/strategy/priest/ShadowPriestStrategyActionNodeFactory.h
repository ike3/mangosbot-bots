#pragma once

namespace ai
{
    class ShadowPriestStrategyActionNodeFactory : public NamedObjectFactory<ActionNode>
    {
    public:
        ShadowPriestStrategyActionNodeFactory()
        {
            creators["mind blast"] = &mind_blast;
            creators["dispersion"] = &dispersion;
        }
    private:
        static ActionNode* mind_blast(PlayerbotAI* ai)
        {
            return new ActionNode ("mind blast",
                /*P*/ NULL,
                /*A*/ NextAction::array(0, new NextAction("mind flay"), NULL),
                /*C*/ NULL);
        }
        static ActionNode* dispersion(PlayerbotAI* ai)
        {
            return new ActionNode ("dispersion",
                /*P*/ NULL,
                /*A*/ NextAction::array(0, new NextAction("mana potion"), NULL),
                /*C*/ NULL);
        }
    };
};
