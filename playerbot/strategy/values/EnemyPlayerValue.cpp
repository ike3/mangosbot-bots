#include "botpch.h"
#include "../../playerbot.h"
#include "EnemyPlayerValue.h"
#include "TargetValue.h"

using namespace ai;
using namespace std;

class FindEnemyPlayerStrategy : public FindTargetStrategy
{
public:
    FindEnemyPlayerStrategy(PlayerbotAI* ai) : FindTargetStrategy(ai)
    {
    }

public:
    virtual void CheckAttacker(Unit* attacker, ThreatManager* threatManager)
    {
        if (!result)
        {
            Player* enemy = dynamic_cast<Player*>(attacker);
            if (enemy &&
                    ai->IsOpposing(enemy) &&
                    enemy->IsPvP() &&
					!sPlayerbotAIConfig.IsInPvpProhibitedZone(enemy->GetAreaId()) &&
                    !enemy->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE) &&
                    !enemy->HasStealthAura() &&
                    !enemy->HasInvisibilityAura() &&
                    //!(enemy->InBattleGround() && ai->GetBot()->GetDistance(enemy) > 40.0f) &&
                    !(enemy->HasAuraType(SPELL_AURA_SPIRIT_OF_REDEMPTION)))
                result = attacker;
        }
    }

};


Unit* EnemyPlayerValue::Calculate()
{
    FindEnemyPlayerStrategy strategy(ai);
    return FindTarget(&strategy);
}
