#include "Counter.h"

static float LOW_ENEMY_ELIXIR = 5.5f;
static float HIGH_IMBALANCE = 40.0f;
static float LOW_THREAT = 20.0f;
static float COUNTER_Y = 8.0f;
static float ARCHER_OFFSET = 1.0F;
static float BAIT_TIMEOUT = 10.0f;

bool Counter::shouldExecute(GameState state, float elixir, float enemyElixir) {


	// The enemy has most of their strength on one side and we have elixir enough to deal with it
	if (abs(state.leftOpossingStrength - state.rightOpossingStrength) > HIGH_IMBALANCE && elixir > 8.0f && enemyElixir < 5.0f) {
		return true;
	}

	// The enemy has a lot of elixir, but not much strength
	if (enemyElixir > 7.0f && state.leftOpossingStrength + state.rightOpossingStrength <= LOW_THREAT) {
		return true;
	}

	return false;
}

bool Counter::execute(iPlayer* player, float deltaTSec, GameState state, float elixir, float enemyElixir) {

	// Waiting for a response to the bait.
	if (baitTimer > 0.0f) {
		baitTimer += deltaTSec;
		// Should mean a response.
		if (enemyElixir < 5.5f) {
			if (elixir >= 8.0f) {
				Vec2 giantPos = Vec2((state.leftOpossingStrength > state.rightOpossingStrength) ? RIGHT_BRIDGE_CENTER_X : LEFT_BRIDGE_CENTER_X,
					player->isNorth() ? COUNTER_Y : GAME_GRID_HEIGHT - COUNTER_Y);
				player->placeMob(iEntityStats::MobType::Giant, giantPos);
				Vec2 archerPos = Vec2(giantPos.x, player->isNorth() ? (giantPos.y - ARCHER_OFFSET) : (giantPos.y + ARCHER_OFFSET));
				player->placeMob(iEntityStats::MobType::Archer, archerPos);
				baitTimer = 0.0f;
				return false;
			}
		}

		if (baitTimer > BAIT_TIMEOUT) {
			std::cout << "Timed out" << std::endl;
			baitTimer = 0.0f;
			return false;
		}

		return true;
	}

	// Enemy has a high imbalance, try to hit them on the other side.
	if (abs(state.leftOpossingStrength - state.rightOpossingStrength) > HIGH_IMBALANCE) {
		if (elixir >= 8.0f) {
			Vec2 giantPos = Vec2((state.leftOpossingStrength > state.rightOpossingStrength) ? RIGHT_BRIDGE_CENTER_X : LEFT_BRIDGE_CENTER_X,
				player->isNorth() ? COUNTER_Y : GAME_GRID_HEIGHT - COUNTER_Y);
			player->placeMob(iEntityStats::MobType::Giant, giantPos);
			return false; // Let someone else deal with the other side
		}
	}

	// Bait and switch
	if (enemyElixir > 8.0f && state.leftOpossingStrength + state.rightOpossingStrength <= LOW_THREAT) {
		if (elixir > 3.0f) {
			// Bait the weaker side to give more incentive for a response
			Vec2 swordmanPos = Vec2((state.leftOpossingStrength > state.rightOpossingStrength) ? RIGHT_BRIDGE_CENTER_X : LEFT_BRIDGE_CENTER_X,
				player->isNorth() ? COUNTER_Y : GAME_GRID_HEIGHT - COUNTER_Y);
			player->placeMob(iEntityStats::MobType::Swordsman, swordmanPos);
			baitTimer = deltaTSec;
			return true;
		}
	}

	return false;
}