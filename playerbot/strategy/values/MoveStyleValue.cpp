#include "botpch.h"
#include "../../playerbot.h"
#include "MoveStyleValue.h"
#include "../values/ItemUsageValue.h"

using namespace ai;
using namespace std;

bool MoveStyleValue::HasValue(PlayerbotAI* ai, const string& value)
{
    string styles = ai->GetAiObjectContext()->GetValue<string>("move style")->Get();
    return styles.find(value) != string::npos;
}