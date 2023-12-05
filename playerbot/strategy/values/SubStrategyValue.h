#pragma once
#include "../Value.h"

namespace ai
{    
    class SubStrategyValue : public ManualSetValue<string>
	{
	public:
        SubStrategyValue(PlayerbotAI* ai, string defaultValue = "", string name = "substrategy", string allowedValues = "") : ManualSetValue(ai, defaultValue, name), allowedValues(allowedValues) {};
        virtual void Set(string newValue) override;

        void SetValues(string& currentValue, const string newValue, const string allowedValue);
        virtual vector<string> GetAllowedValues() { return StrSplit(allowedValues, ","); }

        virtual string Save() override { return value; };
        virtual bool Load(string value) override;
    private:
        string allowedValues;
    };
};

