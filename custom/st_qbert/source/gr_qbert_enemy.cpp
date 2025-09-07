#include <memory.h>
#include "gr_qbert_enemy.h"
#include <OS/OSError.h>
#include <mt/mt_prng.h>
#include <mt/mt_trig.h>
#include <mt/mt_common.h>

float distance(Vec3f* p1, Vec3f* p2) {
    float deltaX = p2->m_x - p1->m_x;
    float deltaY = p2->m_y - p1->m_y;
    float deltaZ = p2->m_z - p1->m_z;
    return mtSqrtf(deltaX*deltaX + deltaY*deltaY + deltaZ*deltaZ);
}

void grQbertEnemy::setTeamScoresWork(u32* teamScoresWork, GameRule gameRule) {
    this->teamScoresWork = teamScoresWork;
    this->gameRule = gameRule;
}

void grQbertEnemy::setStart() {
    this->setSleepHit(false);
    this->setSleepAttack(false);
    this->setNodeVisibility(true, 0, "EnemyM", false, false);
    this->setMotionRatio(1.0);
    this->setRot(0, 0, 0);
    this->isStart = true;
    this->isDead = false;
    this->setStartPos();
}

void grQbertEnemy::update(float frameDelta) {
    if (this->immobilizeTimer > 0) {
        this->immobilizeTimer -= frameDelta;
        if (this->immobilizeTimer <= 0) {
            this->setMotionRatio(1.0);
            if (!this->isDead) {
                this->setSleepHit(false);
                this->setSleepAttack(false);
            }
        }
    }
    else {
        if (this->isStart) {
            this->updateShake(frameDelta);
            this->updateMove(frameDelta);
        }
        grMadein::update(frameDelta);
    }
}

void grQbertEnemy::updateShake(float frameDelta) {
    this->shakeTimer -= frameDelta;
    if (this->shakeTimer <= 0) {
        this->shakeTimer = 0;
        this->shakeOffset = Vec3f(0, 0, 0);
    }
    else {
        if ((u32)this->shakeTimer % 3 == 0) {
            float x;
            float y;
            mtSinCosf(0, &y, &x);
            float shakeMul = 0.5 + 0.8*randf();
            this->shakeOffset = Vec3f(shakeMul*x, shakeMul*y, 0);
        }
    }
}

void grQbertEnemy::setAnim() {
    Vec3f deltaPos = this->targetPos - this->prevPos;

    // pick jump animation based on direction
    if (deltaPos.m_x >= 0 && deltaPos.m_y >= 0) {
        this->setMotion(3);
    }
    else if (deltaPos.m_x < 0 && deltaPos.m_y >= 0) {
        this->setMotion(2);
    }
    else if (deltaPos.m_x < 0 && deltaPos.m_y < 0) {
        this->setMotion(0);
    }
    else if (deltaPos.m_x >= 0 && deltaPos.m_y < 0) {
        this->setMotion(1);
    }
}

void grQbertEnemy::setImmobilize(float immobilizeDuration) {
    this->immobilizeTimer = immobilizeDuration;
    this->setMotionRatio(0.0);
    this->setSleepHit(true);
    this->setSleepAttack(true);
}
