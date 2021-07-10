#pragma once

class PlayerbotAI;

namespace ai
{
    class AiObjectContext;
    class ChatHelper;

    class AiObject : public PlayerbotAIAware
	{
	public:
        AiObject(PlayerbotAI* ai);

    protected:
        Player* bot;
        Player* GetMaster();
        AiObjectContext* context;
        ChatHelper* chat;
	};

    class AiNamedObject : public AiObject
    {
    public:
        AiNamedObject(PlayerbotAI* ai, string name) : AiObject(ai), name(name) {}

    public:
        virtual string getName() { return name; }

    protected:
        string name;
    };
}

// MACROS GO HERE

//
// TRIGGERS
//

#define NEXT_TRIGGERS(name, relevance) \
    virtual NextAction* getNextAction() { return new NextAction(name, relevance); }

#define BEGIN_TRIGGER(clazz, super) \
class clazz : public super \
    { \
    public: \
        clazz(PlayerbotAI* ai) : super(ai) {} \
    public: \
        virtual bool IsActive();

#define END_TRIGGER() \
    };

#define BUFF_TRIGGER(clazz, spell) \
    class clazz : public BuffTrigger \
    { \
    public: \
        clazz(PlayerbotAI* ai) : BuffTrigger(ai, spell) {} \
    }

#define BUFF_TRIGGER_A(clazz, spell) \
    class clazz : public BuffTrigger \
    { \
    public: \
        clazz(PlayerbotAI* ai) : BuffTrigger(ai, spell) {} \
        virtual bool IsActive(); \
    }

#define BUFF_ON_PARTY_TRIGGER(clazz, spell) \
    class clazz : public BuffOnPartyTrigger \
    { \
    public: \
        clazz(PlayerbotAI* ai) : BuffOnPartyTrigger(ai, spell) {}  \
    }

#define BUFF_ON_PARTY_TRIGGER_A(clazz, spell) \
    class clazz : public BuffOnPartyTrigger \
    { \
    public: \
        clazz(PlayerbotAI* ai) : BuffOnPartyTrigger(ai, spell) {}  \
        virtual bool IsActive(); \
    }

#define DEBUFF_TRIGGER(clazz, spell) \
    class clazz : public DebuffTrigger \
    { \
    public: \
        clazz(PlayerbotAI* ai) : DebuffTrigger(ai, spell) {} \
    }

#define DEBUFF_TRIGGER_A(clazz, spell) \
    class clazz : public DebuffTrigger \
    { \
    public: \
        clazz(PlayerbotAI* ai) : DebuffTrigger(ai, spell) {} \
        virtual bool IsActive(); \
    }

#define DEBUFF_ENEMY_TRIGGER(clazz, spell) \
    class clazz : public DebuffOnAttackerTrigger \
    { \
    public: \
        clazz(PlayerbotAI* ai) : DebuffOnAttackerTrigger(ai, spell) {} \
    }

#define DEBUFF_ENEMY_TRIGGER_A(clazz, spell) \
    class clazz : public DebuffOnAttackerTrigger \
    { \
    public: \
        clazz(PlayerbotAI* ai) : DebuffOnAttackerTrigger(ai, spell) {} \
        virtual bool IsActive(); \
    }

#define CAN_CAST_TRIGGER(clazz, spell) \
    class clazz : public SpellCanBeCastTrigger \
    { \
    public: \
        clazz(PlayerbotAI* ai) : SpellCanBeCastTrigger(ai, spell) {} \
    }

#define CAN_CAST_TRIGGER_A(clazz, spell) \
    class clazz : public SpellCanBeCastTrigger \
    { \
    public: \
        clazz(PlayerbotAI* ai) : SpellCanBeCastTrigger(ai, spell) {} \
        virtual bool IsActive(); \
    }

#define INTERRUPT_TRIGGER(clazz, spell) \
    class clazz : public InterruptSpellTrigger \
    { \
    public: \
        clazz(PlayerbotAI* ai) : InterruptSpellTrigger(ai, spell) {} \
    }

#define INTERRUPT_TRIGGER_A(clazz, spell) \
    class clazz : public InterruptSpellTrigger \
    { \
    public: \
        clazz(PlayerbotAI* ai) : InterruptSpellTrigger(ai, spell) {} \
        virtual bool IsActive(); \
    }

#define HAS_AURA_TRIGGER(clazz, spell) \
    class clazz : public HasAuraTrigger \
    { \
    public: \
        clazz(PlayerbotAI* ai) : HasAuraTrigger(ai, spell) {} \
    }

#define HAS_AURA_TRIGGER_A(clazz, spell) \
    class clazz : public HasAuraTrigger \
    { \
    public: \
        clazz(PlayerbotAI* ai) : HasAuraTrigger(ai, spell) {} \
        virtual bool IsActive(); \
    }

#define SNARE_TRIGGER(clazz, spell) \
    class clazz : public SnareTargetTrigger \
    { \
    public: \
        clazz(PlayerbotAI* ai) : SnareTargetTrigger(ai, spell) {} \
    }

#define SNARE_TRIGGER_A(clazz, spell) \
    class clazz : public SnareTargetTrigger \
    { \
    public: \
        clazz(PlayerbotAI* ai) : SnareTargetTrigger(ai, spell) {} \
        virtual bool IsActive(); \
    }

#define PROTECT_TRIGGER(clazz, spell) \
    class clazz : public ProtectPartyMemberTrigger \
    { \
    public: \
        clazz(PlayerbotAI* ai) : ProtectPartyMemberTrigger(ai) {} \
    }

#define BOOST_TRIGGER(clazz, spell) \
    class clazz : public BoostTrigger \
    { \
    public: \
        clazz(PlayerbotAI* ai) : BoostTrigger(ai, spell) {} \
    }

#define BOOST_TRIGGER_A(clazz, spell) \
    class clazz : public BoostTrigger \
    { \
    public: \
        clazz(PlayerbotAI* ai) : BoostTrigger(ai, spell) {} \
        virtual bool IsActive(); \
    }

#define INTERRUPT_HEALER_TRIGGER(clazz, spell) \
    class clazz : public InterruptEnemyHealerTrigger \
    { \
    public: \
        clazz(PlayerbotAI* ai) : InterruptEnemyHealerTrigger(ai, spell) {} \
    }

#define INTERRUPT_HEALER_TRIGGER_A(clazz, spell) \
    class clazz : public InterruptEnemyHealerTrigger \
    { \
    public: \
        clazz(PlayerbotAI* ai) : InterruptEnemyHealerTrigger(ai, spell) {} \
        virtual bool IsActive(); \
    }

//
// ACTIONS
//

#define MELEE_ACTION(clazz, spell) \
    class clazz : public CastMeleeSpellAction \
    { \
    public: \
        clazz(PlayerbotAI* ai) : CastMeleeSpellAction(ai, spell) {} \
    }

#define MELEE_ACTION_U(clazz, spell, useful) \
    class clazz : public CastMeleeSpellAction \
    { \
    public: \
        clazz(PlayerbotAI* ai) : CastMeleeSpellAction(ai, spell) {} \
        virtual bool isUseful() { return useful; } \
    }

#define SPELL_ACTION(clazz, spell) \
    class clazz : public CastSpellAction \
    { \
    public: \
        clazz(PlayerbotAI* ai) : CastSpellAction(ai, spell) {} \
    }

#define BUFF_ACTION(clazz, spell) \
    class clazz : public CastBuffSpellAction \
    { \
    public: \
        clazz(PlayerbotAI* ai) : CastBuffSpellAction(ai, spell) {} \
    }

#define BUFF_ACTION_U(clazz, spell, useful) \
    class clazz : public CastBuffSpellAction \
    { \
    public: \
        clazz(PlayerbotAI* ai) : CastBuffSpellAction(ai, spell) {} \
        virtual bool isUseful() { return useful; } \
    }

#define DEBUFF_ACTION(clazz, spell) \
    class clazz : public CastDebuffSpellAction \
    { \
    public: \
        clazz(PlayerbotAI* ai) : CastDebuffSpellAction(ai, spell) {} \
    }

#define DEBUFF_ACTION_U(clazz, spell, useful) \
    class clazz : public CastDebuffSpellAction \
    { \
    public: \
        clazz(PlayerbotAI* ai) : CastDebuffSpellAction(ai, spell) {} \
        virtual bool isUseful() { return useful; } \
    }

#define DEBUFF_ACTION_R(clazz, spell, distance) \
    class clazz : public CastDebuffSpellAction \
    { \
    public: \
        clazz(PlayerbotAI* ai) : CastDebuffSpellAction(ai, spell) { \
            range = distance; \
        } \
    }

#define DEBUFF_ENEMY_ACTION(clazz, spell) \
    class clazz : public CastDebuffSpellOnAttackerAction \
    { \
    public: \
        clazz(PlayerbotAI* ai) : CastDebuffSpellOnAttackerAction(ai, spell) {} \
    }

#define REACH_ACTION(clazz, spell, range) \
    class clazz : public CastReachTargetSpellAction \
    { \
    public: \
        clazz(PlayerbotAI* ai) : CastReachTargetSpellAction(ai, spell, range) {} \
    }

#define REACH_ACTION_U(clazz, spell, range, useful) \
    class clazz : public CastReachTargetSpellAction \
    { \
    public: \
        clazz(PlayerbotAI* ai) : CastReachTargetSpellAction(ai, spell, range) {} \
        virtual bool isUseful() { return useful; } \
    }

#define ENEMY_HEALER_ACTION(clazz, spell) \
    class clazz : public CastSpellOnEnemyHealerAction \
    { \
    public: \
        clazz(PlayerbotAI* ai) : CastSpellOnEnemyHealerAction(ai, spell) {} \
    }


#define SNARE_ACTION(clazz, spell) \
    class clazz : public CastSnareSpellAction \
    { \
    public: \
        clazz(PlayerbotAI* ai) : CastSnareSpellAction(ai, spell) {} \
    }

#define PROTECT_ACTION(clazz, spell) \
    class clazz : public CastProtectSpellAction \
    { \
    public: \
        clazz(PlayerbotAI* ai) : CastProtectSpellAction(ai, spell) {} \
    }

#define END_RANGED_SPELL_ACTION() \
    };

#define BEGIN_SPELL_ACTION(clazz, name) \
class clazz : public CastSpellAction \
        { \
        public: \
        clazz(PlayerbotAI* ai) : CastSpellAction(ai, name) {} \


#define END_SPELL_ACTION() \
    };

#define BEGIN_DEBUFF_ACTION(clazz, name) \
class clazz : public CastDebuffSpellAction \
        { \
        public: \
        clazz(PlayerbotAI* ai) : CastDebuffSpellAction(ai, name) {} \

#define BEGIN_RANGED_SPELL_ACTION(clazz, name) \
class clazz : public CastSpellAction \
        { \
        public: \
        clazz(PlayerbotAI* ai) : CastSpellAction(ai, name) {} \

#define BEGIN_MELEE_SPELL_ACTION(clazz, name) \
class clazz : public CastMeleeSpellAction \
        { \
        public: \
        clazz(PlayerbotAI* ai) : CastMeleeSpellAction(ai, name) {} \


#define END_RANGED_SPELL_ACTION() \
    };


#define BEGIN_BUFF_ON_PARTY_ACTION(clazz, name) \
class clazz : public BuffOnPartyAction \
        { \
        public: \
        clazz(PlayerbotAI* ai) : BuffOnPartyAction(ai, name) {}

//
// Action node
//

// node_name , action, prerequisite
#define ACTION_NODE_P(name, spell, pre) \
static ActionNode* name(PlayerbotAI* ai) \
    { \
    return new ActionNode(spell, \
        /*P*/ NextAction::array(0, new NextAction(pre), NULL), \
        /*A*/ NULL, \
        /*C*/ NULL); \
    }

// node_name , action, alternative
#define ACTION_NODE_A(name, spell, alt) \
static ActionNode* name(PlayerbotAI* ai) \
    { \
    return new ActionNode(spell, \
        /*P*/ NULL, \
        /*A*/ NextAction::array(0, new NextAction(alt), NULL), \
        /*C*/ NULL); \
    }

// node_name , action, continuer
#define ACTION_NODE_C(name, spell, con) \
static ActionNode* name(PlayerbotAI* ai) \
    { \
    return new ActionNode(spell, \
        /*P*/ NULL, \
        /*A*/ NULL, \
        /*C*/ NextAction::array(0, new NextAction(con), NULL); \
    }
