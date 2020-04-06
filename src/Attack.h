#pragma once

#include "iStrategy.h"

class Attack : public iStrategy {
public:
	Attack() {}
	~Attack() {}

	bool shouldExecute(GameState state, float elixir, float enemyElixir) override;

	bool execute(iPlayer* player, float deltaTSec, GameState state, float elixir, float enemyElixir) override;
};