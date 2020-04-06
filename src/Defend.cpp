#include "Defend.h"

static float ARCHER_OFFSET = 1.0f;
static float DIST_FROM_ENEMY = 5.0f;

static float LOW_DEFENSE = 20.0f;
static float MEDIUM_THREAT = 20.0f;
static float HIGH_THREAT = 35.0f;

bool Defend::shouldExecute(GameState state, float elixir, float enemyElixir) {
	// This isn't really that complicated, intentionally.
	return state.leftOpossingStrength + state.rightOpossingStrength > HIGH_THREAT 
		|| state.leftOpossingStrength + state.rightOpossingStrength > MEDIUM_THREAT && state.leftStrength + state.rightStrength < LOW_DEFENSE;
}

bool Defend::execute(iPlayer* player, float deltaTSec, GameState state, float elixir, float enemyElixir) {

	// This strategy only cares about preventing enemy attacks.
	if (player->getNumOpponentMobs() < 1) {
		return false;
	}

	// We're in an alright spot
	if (state.leftOpossingStrength - state.leftStrength <= 0 && state.rightOpossingStrength - state.rightStrength <= 0) {
		return false;
	}

	bool north = player->isNorth();
	Vec2 closestEnemy = (north) ? Vec2(0.0f, (float)GAME_GRID_HEIGHT) : Vec2(0.0f, 0.0f);
	for (int i = 0; i < player->getNumOpponentMobs(); i++) {
		if (north) {
			if (player->getOpponentMob(i).m_Position.y < closestEnemy.y) {
				closestEnemy = player->getOpponentMob(i).m_Position;
			}
		}
		else {
			if (player->getOpponentMob(i).m_Position.y > closestEnemy.y) {
				closestEnemy = player->getOpponentMob(i).m_Position;
			}
		}
	}

	if (elixir > 2.0f) {

		Vec2 placement = (north) ? Vec2(closestEnemy.x, closestEnemy.y - DIST_FROM_ENEMY) : Vec2(closestEnemy.x, closestEnemy.y + DIST_FROM_ENEMY);
		placement.x += ARCHER_OFFSET;
		player->placeMob(iEntityStats::MobType::Archer, placement);
		if (elixir > 2.0f) {
			placement.x -= ARCHER_OFFSET * 2;
			player->placeMob(iEntityStats::MobType::Archer, placement);
			return false;
		}
		if (elixir > 3.0f) {
			// If we've got extra elixir, throw in a swordsman
			player->placeMob(iEntityStats::MobType::Swordsman, closestEnemy);
		}
	}

	return false;
}