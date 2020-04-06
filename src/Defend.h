#pragma once

#include "iStrategy.h"

class Defend : public iStrategy {
public:
	Defend() {}
	~Defend() {}

	bool shouldExecute(GameState state, float elixir, float enemyElixir) override;

	bool execute(iPlayer* player, float deltaTSec, GameState state, float elixir, float enemyElixir) override;
};

