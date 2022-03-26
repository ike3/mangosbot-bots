#pragma once

class Player;
class PlayerbotMgr;
class ChatHandler;

using namespace std;

class PlayerbotAIBase
{
public:
	PlayerbotAIBase();

public:
	bool CanUpdateAI();
	void SetNextCheckDelay(const uint32 delay);
    void IncreaseNextCheckDelay(uint32 delay);
	void YieldThread(bool delay = false);
    virtual void UpdateAI(uint32 elapsed);
    virtual void UpdateAIInternal(uint32 elapsed, bool minimal = false);
    bool IsActive();

protected:
	uint32 nextAICheckDelay;
};
