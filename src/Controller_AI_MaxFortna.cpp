// MIT License
// 
// Copyright(c) 2020 Arthur Bacon and Kevin Dill
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this softwareand associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and /or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
// 
// The above copyright noticeand this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "Controller_AI_MaxFortna.h"

#include "Constants.h"
#include "EntityStats.h"
#include "iPlayer.h"
#include "Attack.h"
#include "Defend.h"
#include "Counter.h"

static const float HEALTH_STR_DIVISOR = 1000;
static const float DAMAGE_STR_DIVISOR = 10;
static const float TARGET_STR_MULTIPLE = 5;
static const float POSITION_STR_MULTIPLE = 10;
static const float BUILDING_STR_MULTIPLE = 50;

Controller_AI_MaxFortna::Controller_AI_MaxFortna() {
    // Create the strategies to use, in order.
    strategies[0] = new Defend();
    strategies[1] = new Counter();
    strategies[2] = new Attack();

    lockedIndex = -1;
    prevEnemyMobs = 0;
    enemyElixir = STARTING_ELIXIR;
    state.reset();
}

void Controller_AI_MaxFortna::getState() {
    state.reset();

    for (unsigned int i = 0; i < m_pPlayer->getNumOpponentMobs(); i++) {
        iPlayer::EntityData mob = m_pPlayer->getOpponentMob(i);
        iEntityStats::MobType type = mob.m_Stats.getMobType();
        float healthStr = mob.m_Health / HEALTH_STR_DIVISOR;
        float damageStr = (mob.m_Stats.getDamage() * mob.m_Stats.getAttackTime() * std::fmax(1.f, mob.m_Stats.getAttackRange() / 2.f)) / DAMAGE_STR_DIVISOR;
        float targetStr = TARGET_STR_MULTIPLE * (mob.m_Stats.getTargetType() == iEntityStats::TargetType::Building) ? 1.0f : 0.0f;
        float yDistFromCenterNormalized = (!m_pPlayer->isNorth()) ? mob.m_Position.y - BRIDGE_CENTER_Y : BRIDGE_CENTER_Y - mob.m_Position.y;
        yDistFromCenterNormalized /= BRIDGE_CENTER_Y;
        float positionStr = POSITION_STR_MULTIPLE * (yDistFromCenterNormalized >= 0.0f) ? (1.0f * yDistFromCenterNormalized + 1.0f) : (-0.5f * yDistFromCenterNormalized + 0.5f);
        if (mob.m_Position.x < KingX) {
            state.leftOpossingStrength += healthStr + damageStr + targetStr + positionStr;
        }
        else {
            state.rightOpossingStrength += healthStr + damageStr + targetStr + positionStr;
        }
    }

    for (unsigned int i = 0; i < m_pPlayer->getNumMobs(); i++) {
        iPlayer::EntityData mob = m_pPlayer->getMob(i);
        iEntityStats::MobType type = mob.m_Stats.getMobType();
        float healthStr = mob.m_Health / HEALTH_STR_DIVISOR;
        float damageStr = (mob.m_Stats.getDamage() * mob.m_Stats.getAttackTime() * std::fmax(1.f, mob.m_Stats.getAttackRange() / 2.f)) / DAMAGE_STR_DIVISOR;
        float targetStr = TARGET_STR_MULTIPLE * (mob.m_Stats.getTargetType() == iEntityStats::TargetType::Building) ? 1.0f : 0.0f;
        float yDistFromCenterNormalized = (m_pPlayer->isNorth()) ? mob.m_Position.y - BRIDGE_CENTER_Y : BRIDGE_CENTER_Y - mob.m_Position.y;
        yDistFromCenterNormalized /= BRIDGE_CENTER_Y;
        float positionStr = POSITION_STR_MULTIPLE * (yDistFromCenterNormalized >= 0.0f) ? (1.0f * yDistFromCenterNormalized + 1.0f) : (-0.5f * yDistFromCenterNormalized + 0.5f);
        if (mob.m_Position.x < KingX) {
            state.leftStrength += healthStr + damageStr + targetStr + positionStr;
        }
        else {
            state.rightStrength += healthStr + damageStr + targetStr + positionStr;
        }
    }
}

void Controller_AI_MaxFortna::tick(float deltaTSec)
{
    assert(m_pPlayer);

    // Evaluate enemy elixir (note that this doesn't work if an enemy mob dies the same frame another is placed, but hopefully that's very rare).
    int currentNumMobs = m_pPlayer->getNumOpponentMobs();
    for (int i = currentNumMobs - prevEnemyMobs; i > 0; i--) {
        enemyElixir -= m_pPlayer->getOpponentMob(currentNumMobs - i).m_Stats.getElixirCost();
    }
    // Evaluate state - this is likely the most performance intesive part of the AI, 
    // consider making it only run every other frame or something if fps is suffering.
    getState();

    if (lockedIndex == -1) {
        bool executed = false;
        for (int i = 0; i < NUM_STRATEGIES; i++) {
            if (strategies[i]->shouldExecute(state, m_pPlayer->getElixir(), enemyElixir)) {
                if (strategies[i]->execute(m_pPlayer, deltaTSec, state, m_pPlayer->getElixir(), enemyElixir)) {
                    lockedIndex = i;
                }
                executed = true;
                break;
            }
        }
        if (!executed) {
            // Last strategy is default
            if (strategies[NUM_STRATEGIES - 1]->execute(m_pPlayer, deltaTSec, state, m_pPlayer->getElixir(), enemyElixir)) {
                lockedIndex = NUM_STRATEGIES - 1;
            }
        }
    }
    else {
        if (!strategies[lockedIndex]->execute(m_pPlayer, deltaTSec, state, m_pPlayer->getElixir(), enemyElixir)) {
            lockedIndex = -1;
        }
    }

    prevEnemyMobs = currentNumMobs;
    enemyElixir += ELIXIR_PER_SECOND * deltaTSec;
}

