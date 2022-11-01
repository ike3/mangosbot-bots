#pragma once
#include "../Value.h"

namespace ai
{
    class AoePositionValue : public CalculatedValue<WorldLocation>
	{
	public:
        AoePositionValue(PlayerbotAI* ai) : CalculatedValue<WorldLocation>(ai, "aoe position") {}

	public:
        virtual WorldLocation Calculate();
    };

    class AoeCountValue : public CalculatedValue<uint8>
	{
	public:
        AoeCountValue(PlayerbotAI* ai) : CalculatedValue<uint8>(ai, "aoe count") {}

	public:
        virtual uint8 Calculate();
    };

    class HasAreaDebuffValue : public BoolCalculatedValue, public Qualified
    {
    public:
        HasAreaDebuffValue(PlayerbotAI* ai) : BoolCalculatedValue(ai) {}

        Unit* GetTarget()
        {
            AiObjectContext* ctx = AiObject::context;

            return ctx->GetValue<Unit*>(qualifier)->Get();
        }
        virtual bool Calculate();
    };
}
