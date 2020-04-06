#pragma once

#include "iStrategy.h"

class Counter : public iStrategy {
public:
	Counter() { baitTimer = 0.0f; }
	~Counter() {}

	bool shouldExecute(GameState state, float elixir, float enemyElixir) override;

	bool execute(iPlayer* player, float deltaTSec, GameState state, float elixir, float enemyElixir) override;

private:
	float baitTimer;
};
