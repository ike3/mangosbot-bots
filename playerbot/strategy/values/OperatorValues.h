#pragma once
#include "../Value.h"

namespace ai
{
    class BoolAndValue : public BoolCalculatedValue, public Qualified
    {
    public:
        BoolAndValue(PlayerbotAI* ai) : BoolCalculatedValue(ai, "bool and"), Qualified() {}
        virtual bool Calculate();

#ifdef GenerateBotHelp
        virtual string GetHelpName() { return "bool and"; } //Must equal iternal name
        virtual string GetHelpTypeName() { return "operator"; }
        virtual string GetHelpDescription()
        {
            return "This value will return true if all of the values included in the qualifier return true.";
        }
        virtual vector<string> GetUsedValues() { return { }; }
#endif 
    };

    class NotValue : public BoolCalculatedValue, public Qualified
    {
    public:
        NotValue(PlayerbotAI* ai) : BoolCalculatedValue(ai, "not"), Qualified() {}
        virtual bool Calculate();

#ifdef GenerateBotHelp
        virtual string GetHelpName() { return "not"; } //Must equal iternal name
        virtual string GetHelpTypeName() { return "operator"; }
        virtual string GetHelpDescription()
        {
            return "This value will return false if any of the values included in the qualifier return true.";
        }
        virtual vector<string> GetUsedValues() { return { }; }
#endif 
    };
}
