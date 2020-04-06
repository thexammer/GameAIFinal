#include "Attack.h"
#include "Vec2.h"

static const float INITIATE_GIANT_Y = 5.0f;
static const float ATTACKING_STR = 25.0f;
static const float SUSTAIN_ARCHER_Y = 5.0f;
static const float ARCHER_BUFFER = 0.5f;
static const float FIRST_SWORDSMAN_Y = 6.0f;
static const float SECOND_SWORDSMAN_Y = 3.0f;

bool Attack::shouldExecute(GameState state, float elixir, float enemyElixir) {
	// We're basically full on elixir and should do something with it.
	if (elixir > 9.0f) {
		return true;
	}
	// We have an advantage in elixir.
	if (enemyElixir < 2.0f && elixir > 5.0f) {
		return true;
	}
	// The enemy is comparatively weak.
	if (state.leftStrength + state.rightStrength - (state.leftOpossingStrength + state.rightOpossingStrength) > 40.0f) {
		return true;
	}

	return false;
}

bool Attack::execute(iPlayer* player, float deltaTSec, GameState state, float elixir, float enemyElixir) {

	// Start an attack on the weaker side, try to sustain later.
	if (elixir > 9.0f) {
		if (state.leftOpossingStrength > state.rightOpossingStrength) {
			player->placeMob(iEntityStats::MobType::Giant, Vec2(LEFT_BRIDGE_CENTER_X, (player->isNorth()) ? INITIATE_GIANT_Y : GAME_GRID_HEIGHT - INITIATE_GIANT_Y));
			player->placeMob(iEntityStats::MobType::Archer, Vec2(LEFT_BRIDGE_CENTER_X + ARCHER_BUFFER, (player->isNorth()) ? SUSTAIN_ARCHER_Y : GAME_GRID_HEIGHT - SUSTAIN_ARCHER_Y));
			player->placeMob(iEntityStats::MobType::Archer, Vec2(LEFT_BRIDGE_CENTER_X - ARCHER_BUFFER, (player->isNorth()) ? SUSTAIN_ARCHER_Y : GAME_GRID_HEIGHT - SUSTAIN_ARCHER_Y));
			return false;
		}
		else {
			player->placeMob(iEntityStats::MobType::Giant, Vec2(RIGHT_BRIDGE_CENTER_X, (player->isNorth()) ? INITIATE_GIANT_Y : GAME_GRID_HEIGHT - INITIATE_GIANT_Y));
			player->placeMob(iEntityStats::MobType::Archer, Vec2(RIGHT_BRIDGE_CENTER_X + ARCHER_BUFFER, (player->isNorth()) ? SUSTAIN_ARCHER_Y : GAME_GRID_HEIGHT - SUSTAIN_ARCHER_Y));
			player->placeMob(iEntityStats::MobType::Archer, Vec2(RIGHT_BRIDGE_CENTER_X - ARCHER_BUFFER, (player->isNorth()) ? SUSTAIN_ARCHER_Y : GAME_GRID_HEIGHT - SUSTAIN_ARCHER_Y));
			return false;
		}
	}

	// Try to sustain any attack we have on going.
	if (elixir > 4.0f && (state.leftStrength > ATTACKING_STR || state.rightStrength > ATTACKING_STR)) {
		if (state.leftStrength > state.rightStrength) {
			player->placeMob(iEntityStats::MobType::Archer, Vec2(LEFT_BRIDGE_CENTER_X + ARCHER_BUFFER, (player->isNorth()) ? SUSTAIN_ARCHER_Y : GAME_GRID_HEIGHT - SUSTAIN_ARCHER_Y));
			player->placeMob(iEntityStats::MobType::Archer, Vec2(LEFT_BRIDGE_CENTER_X - ARCHER_BUFFER, (player->isNorth()) ? SUSTAIN_ARCHER_Y : GAME_GRID_HEIGHT - SUSTAIN_ARCHER_Y));
			return false;
		}
		else {
			player->placeMob(iEntityStats::MobType::Archer, Vec2(RIGHT_BRIDGE_CENTER_X + ARCHER_BUFFER, (player->isNorth()) ? SUSTAIN_ARCHER_Y : GAME_GRID_HEIGHT - SUSTAIN_ARCHER_Y));
			player->placeMob(iEntityStats::MobType::Archer, Vec2(RIGHT_BRIDGE_CENTER_X - ARCHER_BUFFER, (player->isNorth()) ? SUSTAIN_ARCHER_Y : GAME_GRID_HEIGHT - SUSTAIN_ARCHER_Y));
			return false;
		}
	}

	float leftPrincessHealth = 1.0f;
	float rightPrincessHealth = 1.0f;
	for (unsigned int i = 0; i < player->getNumOpponentBuildings(); i++) {
		iPlayer::EntityData building = player->getOpponentBuilding(i);
		if (building.m_Stats.getBuildingType() == iEntityStats::BuildingType::Princess && building.m_Health > 0) {
			if (building.m_Position.x < KingX) {
				leftPrincessHealth = ((float)building.m_Health / (float)building.m_Stats.getMaxHealth());
			}
			else {
				rightPrincessHealth = ((float)building.m_Health / (float)building.m_Stats.getMaxHealth());
			}
		}
	}
	// Press a weak tower.
	if (elixir > 5.0f && (leftPrincessHealth < 0.25f || rightPrincessHealth < 0.25f)) {
		if (leftPrincessHealth < rightPrincessHealth) {
			player->placeMob(iEntityStats::MobType::Swordsman, Vec2(LEFT_BRIDGE_CENTER_X, (player->isNorth()) ? FIRST_SWORDSMAN_Y : GAME_GRID_HEIGHT - FIRST_SWORDSMAN_Y));
			if (elixir > 2.0f) {
				player->placeMob(iEntityStats::MobType::Archer, Vec2(LEFT_BRIDGE_CENTER_X, (player->isNorth()) ? SECOND_SWORDSMAN_Y : GAME_GRID_HEIGHT - SECOND_SWORDSMAN_Y));
			}
			return false;
		}
		else {
			player->placeMob(iEntityStats::MobType::Swordsman, Vec2(RIGHT_BRIDGE_CENTER_X, (player->isNorth()) ? FIRST_SWORDSMAN_Y : GAME_GRID_HEIGHT - FIRST_SWORDSMAN_Y));
			if (elixir > 3.0f) {
				player->placeMob(iEntityStats::MobType::Archer, Vec2(RIGHT_BRIDGE_CENTER_X, (player->isNorth()) ? SECOND_SWORDSMAN_Y : GAME_GRID_HEIGHT - SECOND_SWORDSMAN_Y));
			}
			return false;
		}
	}

	return false;
}