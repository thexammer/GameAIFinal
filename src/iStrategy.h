#pragma once

#include "iPlayer.h"
#include "Constants.h"

struct GameState {
    float leftOpossingStrength;
    float rightOpossingStrength;
    float leftStrength;
    float rightStrength;

    void reset() {
        leftOpossingStrength = 0;
        rightOpossingStrength = 0;
        leftStrength = 0;
        rightStrength = 0;
    }
};


class iStrategy {
public:
	iStrategy() {}
	virtual ~iStrategy() {}

	// Whether or not the game enviroment satifies our thresholds.
	virtual bool shouldExecute(GameState state, float elixir, float enemyElixir) = 0;

	// This is the current strategy, use the game enviroment (arguments) to decide what to do.
	// Note that shouldExecute does not have to return true for this to run, so all cases should be accounted for.
	// Returns a bool where a true asks the controller to lock in this strategy so that it can run over multiple ticks.
	virtual bool execute(iPlayer* player, float deltaTSec, GameState state, float elixir, float enemyElixir) = 0;
};