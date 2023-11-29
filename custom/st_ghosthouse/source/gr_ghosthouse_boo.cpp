#include "gr_ghosthouse_boo.h"
#include <ec/ec_mgr.h>
#include <memory.h>
#include <ft/ft_manager.h>
#include <hk/hk_math.h>
#include <mt/mt_prng.h>
#include <mt/mt_trig.h>
#include <OS/OSError.h>
#include <math.h>
#include <gr/gr_gimmick_motion_path.h>
#include <st/st_utility.h>

grGhostHouseBoo* grGhostHouseBoo::create(int mdlIndex, const char* tgtNodeName, const char* taskName)
{
    grGhostHouseBoo* boo = new (Heaps::StageInstance) grGhostHouseBoo(taskName);
    boo->setMdlIndex(mdlIndex);
    boo->m_heapType = Heaps::StageInstance;
    boo->makeCalcuCallback(1, Heaps::StageInstance);
    boo->setCalcuCallbackRoot(7);
    boo->setupMelee();

    boo->m_isMotionPathTranslate = true;

    return boo;
}

void grGhostHouseBoo::update(float deltaFrame)
{
    grMadein::update(deltaFrame);
    this->updateMove(deltaFrame);
}

void grGhostHouseBoo::setupAttack() {
    stGhostHouseData* ghostHouseData = static_cast<stGhostHouseData*>(this->getStageData());

    float size = 1.0;
    Vec3f offsetPos = {0.0, 0.0, 0.0};
    this->setAttack(size, &offsetPos);
    this->m_attackInfo->m_preset = 4;

    soCollisionAttackData* overwriteAttackData = this->getOverwriteAttackData();
    this->createAttackPointNormal(overwriteAttackData);
    overwriteAttackData->m_reactionEffect = ghostHouseData->booReactionEffect;
    overwriteAttackData->m_reactionFix = ghostHouseData->booReactionFix;
    overwriteAttackData->m_reactionAdd = ghostHouseData->booReactionAdd;
    overwriteAttackData->m_power = ghostHouseData->booPower;
    overwriteAttackData->m_vector = ghostHouseData->booVector;
    overwriteAttackData->m_size = 1.0;
    overwriteAttackData->m_offsetPos = offsetPos;
    overwriteAttackData->m_hitstopMultiplier = ghostHouseData->booHitstopMultiplier;

    overwriteAttackData->m_bits.nodeIndex = 0x4;

    overwriteAttackData->m_bits.isCollisionCategory9 = true;
    overwriteAttackData->m_bits.isCollisionCategory8 = true;
    overwriteAttackData->m_bits.isCollisionCategory7 = true;
    overwriteAttackData->m_bits.isCollisionCategory6 = true;
    overwriteAttackData->m_bits.isCollisionCategory5 = true;
    overwriteAttackData->m_bits.isCollisionCategory4 = true;
    overwriteAttackData->m_bits.isCollisionCategory3 = true;
    overwriteAttackData->m_bits.isCollisionCategory2 = true;
    overwriteAttackData->m_bits.isCollisionCategory1 = true;
    overwriteAttackData->m_bits.isCollisionCategory0 = true;

    overwriteAttackData->m_bits.isCollisionSituationODD = true;
    overwriteAttackData->m_bits.isCollisionSituationAir = true;
    overwriteAttackData->m_bits.isCollisionSituationGround = true;

    overwriteAttackData->m_bits.field_0x30_3 = false;
    overwriteAttackData->m_bits.isCollisionPartRegion3 = true;
    overwriteAttackData->m_bits.isCollisionPartRegion2 = true;
    overwriteAttackData->m_bits.isCollisionPartRegion1 = true;
    overwriteAttackData->m_bits.isCollisionPartRegion0 = true;
    overwriteAttackData->m_bits.elementType = Element_Type_Normal;

    overwriteAttackData->m_bits.hitSoundLevel = Hit_Sound_Level_Small;
    overwriteAttackData->m_bits.hitSoundType = Hit_Sound_Type_Punch;
    overwriteAttackData->m_bits.isClankable = false;
    overwriteAttackData->m_bits.field_0x34_3 = false;
    overwriteAttackData->m_bits.field_0x34_4 = false;
    overwriteAttackData->m_bits.isBlockable = true;
    overwriteAttackData->m_bits.isReflectable = false;
    overwriteAttackData->m_bits.isAbsorbable = false;
    overwriteAttackData->m_bits.field_0x38_10 = 0;

    overwriteAttackData->m_bits.detectionRate = ghostHouseData->booDetectionRate;
    overwriteAttackData->m_bits.field_0x38_1 = false;
    overwriteAttackData->m_bits.ignoreInvincibility = false;
    overwriteAttackData->m_bits.ignoreIntangibility = false;
    overwriteAttackData->m_bits.facingRestriction = Facing_Restriction_Normal;
    overwriteAttackData->m_bits.field_0x38_5 = false;
    overwriteAttackData->m_bits.enableFriendlyFire = false;
    overwriteAttackData->m_bits.disableHitstop = false;
    overwriteAttackData->m_bits.disableHitGfx = false;
    overwriteAttackData->m_bits.disableFlinch = false;
    overwriteAttackData->m_bits.addedShieldDamage = ghostHouseData->booShieldDamage;

    overwriteAttackData->m_bits.isShapeCapsule = true;
}

void grGhostHouseBoo::updateMove(float deltaFrame) {
    stGhostHouseData* ghostHouseData = static_cast<stGhostHouseData*>(this->getStageData());
    float currentAnimFrame = this->m_modelAnims[0]->getFrame();
    float animFrameCount = this->m_modelAnims[0]->getFrameCount();
    switch(this->state) {
        case State_CircleStart:
        {
            animFrameCount = this->m_modelAnims[0]->m_anmObjMatClrRes->m_anmMatClrFile->m_animLength;
            if (currentAnimFrame >= animFrameCount - 1) {
                this->changeState(State_Circle);
            }
            Vec3f closestDisp;
            this->findClosestFighterDisp(&closestDisp);
            this->rotateToDisp(&closestDisp.m_xy, ghostHouseData->booRot, deltaFrame * BOO_ROT_SPEED);
        }
            break;
        case State_Circle:
        {
            if (this->m_gimmickMotionPath == NULL) {
                this->setPos(this->circleCenterPos.m_x + this->circleRadius*cos(this->circleCurrentAngle), this->circleCenterPos.m_y + this->circleRadius*sin(this->circleCurrentAngle), 0);
                this->circleCurrentAngle += deltaFrame*mtConvDegToRad(this->speed);

                if (this->circleCurrentAngle >= 2*M_PI) {
                    this->circleCurrentAngle -= 2*M_PI;
                }
                else if (this->circleCurrentAngle <= -2*M_PI) {
                    this->circleCurrentAngle += 2*M_PI;
                }
            }

            Vec3f closestDisp;
            this->findClosestFighterDisp(&closestDisp);
            this->rotateToDisp(&closestDisp.m_xy, ghostHouseData->booRot, deltaFrame*BOO_ROT_SPEED);
        }
            break;
        case State_SnakeStart:
            animFrameCount = this->m_modelAnims[0]->m_anmObjMatClrRes->m_anmMatClrFile->m_animLength;
            if (currentAnimFrame >= animFrameCount - 1) {
                this->changeState(State_Snake);
            }
            break;
        case State_Snake:
            if (this->snakeLeader == NULL) {
                Vec3f pos = this->getPos();
                Vec3f hurtNodeScale;
                this->getNodeScale(&hurtNodeScale, 0, "Hurt");
                Vec3f dir = (Vec3f) {this->direction.m_x, this->direction.m_y, 0.0} * hurtNodeScale.m_x;
                Vec3f outHitPos;
                Vec3f outCollNormalVec;

                if (stRayCheck(&pos, &dir, &outHitPos, &outCollNormalVec, 1, NULL, 0, 5)) {
                    float dirAngle = atan2(this->direction.m_y, this->direction.m_x) + M_PI;
                    float normalAngle = atan2(outCollNormalVec.m_y, outCollNormalVec.m_x) + M_PI;
                    float rotAngle = mtConvRadToDeg(atan2(BOO_SNAKE_DIR_Y, BOO_SNAKE_DIR_X));

                    if (0 < dirAngle && M_PI / 2 >= dirAngle) { // quad 1
                        if (7 * M_PI / 6 <= normalAngle && 4 * M_PI / 3 >= normalAngle) {
                            this->direction = (Vec2f) {BOO_SNAKE_DIR_X, BOO_SNAKE_DIR_Y};
                            this->setRot(-rotAngle, ghostHouseData->booRot, 0);
                        } else if (5 * M_PI / 4 < normalAngle && 7 * M_PI / 4 >= normalAngle) {
                            this->direction = (Vec2f) {-BOO_SNAKE_DIR_X, BOO_SNAKE_DIR_Y};
                            this->setRot(-rotAngle, -ghostHouseData->booRot, 0);
                        } else if (3 * M_PI / 4 < normalAngle && 5 * M_PI / 4 >= normalAngle) {
                            this->direction = (Vec2f) {BOO_SNAKE_DIR_X, -BOO_SNAKE_DIR_Y};
                            this->setRot(rotAngle, ghostHouseData->booRot, 0);
                        }
                    } else if (M_PI / 2 < dirAngle && M_PI >= dirAngle) {   // quad 2
                        if (5 * M_PI / 3 <= normalAngle && 11 * M_PI / 6 >= normalAngle) {
                            this->direction = (Vec2f) {-BOO_SNAKE_DIR_X, BOO_SNAKE_DIR_Y};
                            this->setRot(-rotAngle, -ghostHouseData->booRot, 0);
                        } else if (5 * M_PI / 4 < normalAngle && 7 * M_PI / 4 >= normalAngle) {
                            this->direction = (Vec2f) {BOO_SNAKE_DIR_X, BOO_SNAKE_DIR_Y};
                            this->setRot(-rotAngle, ghostHouseData->booRot, 0);
                        } else if (7 * M_PI / 4 < normalAngle || M_PI / 4 >= normalAngle) {
                            this->direction = (Vec2f) {-BOO_SNAKE_DIR_X, -BOO_SNAKE_DIR_Y};
                            this->setRot(rotAngle, -ghostHouseData->booRot, 0);
                        }
                    } else if (M_PI < dirAngle && 3 * M_PI / 2 >= dirAngle) { // quad 3
                        if (M_PI / 6 <= normalAngle && M_PI / 3 >= normalAngle) {
                            this->direction = (Vec2f) {-BOO_SNAKE_DIR_X, -BOO_SNAKE_DIR_Y};
                            this->setRot(rotAngle, -ghostHouseData->booRot, 0);
                        } else if (M_PI / 4 < normalAngle && 3 * M_PI / 4 >= normalAngle) {
                            this->direction = (Vec2f) {BOO_SNAKE_DIR_X, -BOO_SNAKE_DIR_Y};
                            this->setRot(rotAngle, ghostHouseData->booRot, 0);
                        } else if (7 * M_PI / 4 < normalAngle || M_PI / 4 >= normalAngle) {
                            this->direction = (Vec2f) {-BOO_SNAKE_DIR_X, BOO_SNAKE_DIR_Y};
                            this->setRot(-rotAngle, -ghostHouseData->booRot, 0);
                        }
                    } else if (3 * M_PI / 2 < dirAngle && 2 * M_PI >= dirAngle) {   // quad 4
                        if (2 * M_PI / 3 <= normalAngle && 5 * M_PI / 6 >= normalAngle) {
                            this->direction = (Vec2f) {BOO_SNAKE_DIR_X, -BOO_SNAKE_DIR_Y};
                            this->setRot(rotAngle, ghostHouseData->booRot, 0);
                        } else if (M_PI / 4 < normalAngle && 3 * M_PI / 4 >= normalAngle) {
                            this->direction = (Vec2f) {-BOO_SNAKE_DIR_X, -BOO_SNAKE_DIR_Y};
                            this->setRot(rotAngle, -ghostHouseData->booRot, 0);
                        } else if (3 * M_PI / 4 < normalAngle && 5 * M_PI / 4 >= normalAngle) {
                            this->direction = (Vec2f) {BOO_SNAKE_DIR_X, BOO_SNAKE_DIR_Y};
                            this->setRot(-rotAngle, ghostHouseData->booRot, 0);
                        }
                    }
                }

                dir = (Vec3f) {this->direction.m_x, this->direction.m_y, 0.0};

                Vec3f nextPos = pos + dir * ghostHouseData->booSnakeSpeed * deltaFrame;
                this->setPos(&nextPos);
            }
            else {
                this->timer -= deltaFrame;
                if (this->timer <= 0) {
                    this->timer = this->maxSnakeTimer;
                    Vec3f leaderPos = this->snakeLeader->getPos();
                    this->setPos(&leaderPos);
                    Vec3f leaderRot = this->snakeLeader->getRot();
                    this->setRot(&leaderRot);
                }
            }
            break;
        case State_CrewStart:
        case State_ChaseFinish:
            if (currentAnimFrame >= animFrameCount - 1) {
                this->changeState(State_Crew);
            }
        case State_Crew:
            {
                Vec3f pos = this->getPos();

                this->speed += this->accel * deltaFrame;
                if (this->speed <= 0) {
                    this->speed = 0;
                    this->accel = ghostHouseData->booCrewIdleAccel;
                    this->direction.m_x = -this->direction.m_x;
                }
                else if (this->speed >= ghostHouseData->booCrewIdleTopSpeed) {
                    this->speed = ghostHouseData->booCrewIdleTopSpeed;
                }

                this->remainingDistance -= ghostHouseData->booCrewDirection.m_y * this->speed * deltaFrame;
                if (this->remainingDistance <= 0) {
                    this->direction.m_y = -this->direction.m_y;
                    this->remainingDistance = ghostHouseData->booCloudIdleMaxVerticalDistance;
                }
                Vec2f newPos = pos.m_xy + this->direction * this->speed * deltaFrame;
                if (this->accel > 0 && ((this->direction.m_x < 0 && newPos.m_x <= this->southWestPos->m_x) ||
                    (this->direction.m_x > 0 && newPos.m_x >= this->northEastPos->m_x) ||
                    (newPos.m_x > this->southWestPos->m_x && newPos.m_x < this->northEastPos->m_x && ghostHouseData->booCrewIdleHorizontalTurnChance > randf()))) {
                    this->accel = -ghostHouseData->booCrewIdleAccel;
                }
                this->setPos(newPos.m_x, newPos.m_y, 0.0);
                this->rotateToDisp(&this->direction, ghostHouseData->booRot, deltaFrame * BOO_ROT_SPEED);

                if (this->state == State_Crew && ghostHouseData->booCrewDetectChance > randf()) {
                    Vec3f closestFighterDisp;
                    if (this->findClosestFighterDisp(&closestFighterDisp) &&
                        fabsf(closestFighterDisp.m_x) <= ghostHouseData->booCrewDetectRange.m_x / 2 &&
                        closestFighterDisp.m_y <= 0.0 &&
                        fabsf(closestFighterDisp.m_y) <= ghostHouseData->booCrewDetectRange.m_y) {
                        pos = this->getPos();
                        Vec2f targetPos = pos.m_xy + closestFighterDisp.m_xy;
                        this->setChase(&pos, &targetPos);
                    }
                }
            }
            break;
        case State_ChaseStart:
        {
            if (currentAnimFrame >= animFrameCount - 1) {
                this->changeState(State_Chase);
            }
            Vec2f disp = this->targetPos - this->getPos().m_xy;
            this->rotateToDisp(&disp, ghostHouseData->booRot, deltaFrame * BOO_ROT_SPEED);
        }
            break;
        case State_Chase:
        {

            Vec3f currentPos = this->getPos();

            this->timer -= deltaFrame;
            if (this->timer < 0) {
                this->timer = 0;
                this->changeState(State_ChaseFinish);
            }

            float speedX = (targetPos.m_x - prevPos.m_x) / ghostHouseData->booCrewChaseFramesToReach;
            float timeElapsed = ghostHouseData->booCrewChaseFramesToReach*2 - this->timer;

            float initSpeedY = 2*(targetPos.m_y - prevPos.m_y) / ghostHouseData->booCrewChaseFramesToReach;
            float accelY = -initSpeedY / ghostHouseData->booCrewChaseFramesToReach;

            Vec3f newPos = (Vec3f){currentPos.m_x + deltaFrame*speedX,
                                   this->prevPos.m_y + initSpeedY*timeElapsed + 0.5*accelY*timeElapsed*timeElapsed,
                                   0.0};

            this->setPos(&newPos);
            Vec2f disp = this->targetPos - newPos.m_xy;
            this->rotateToDisp(&disp, ghostHouseData->booRot, deltaFrame * BOO_ROT_SPEED);
        }
            break;
        case State_AppearStart:
            if (currentAnimFrame >= animFrameCount - 1) {
                this->changeState(State_Appear);
            }
        case State_Appear:
        {
            Vec3f closestDisp;
            this->findClosestFighterDisp(&closestDisp);
            this->rotateToDisp(&closestDisp.m_xy, ghostHouseData->booRot, deltaFrame * BOO_ROT_SPEED);

            this->timer -= deltaFrame;
            if (this->timer <= 0) {
                this->changeState(State_Disappear);
            }
        }
            break;
        case State_Disappear:
        {
            Vec3f closestDisp;
            this->findClosestFighterDisp(&closestDisp);
            this->rotateToDisp(&closestDisp.m_xy, ghostHouseData->booRot, deltaFrame * BOO_ROT_SPEED);
            this->timer -= deltaFrame;
            if (this->timer <= 0) {
                this->changeState(State_AppearStart);
            }
        }
            break;
        case State_StalkStart:
            if (currentAnimFrame >= animFrameCount - 1) {
                this->changeState(State_Stalk);
            }
            break;
        case State_ShyStart:
            if (currentAnimFrame >= animFrameCount - 1) {
                this->changeState(State_Shy);
            }
        case State_Shy:
        case State_Stalk:
            for (int i = 0; i < g_ftManager->getEntryCount(); i++) {
                int entryId = g_ftManager->getEntryIdFromIndex(i);
                if (this->playerTarget == g_ftManager->getPlayerNo(entryId) && g_ftManager->isFighterActivate(entryId, -1)) {

                    Vec3f targetFighterPos = g_ftManager->getFighterCenterPos(entryId, -1);
                    Vec3f dirVec = targetFighterPos - this->getPos();
                    float targetFighterLr = g_ftManager->getFighterLr(entryId, -1);
                    if (dirVec.m_x >= 0) {
                        this->setRot(0, ghostHouseData->booRot, 0);
                        if (targetFighterLr < 0) {
                            this->changeState(State_ShyStart);
                        }
                        else {
                            this->changeState(State_Stalk);
                        }
                    }
                    else {
                        this->setRot(0, -ghostHouseData->booRot, 0);
                        if (targetFighterLr > 0) {
                            this->changeState(State_ShyStart);
                        }
                        else {
                            this->changeState(State_Stalk);
                        }
                    }
                    if (this->state == State_Stalk) {
                        Vec3f currentPos = this->getPos();
                        this->speed += ghostHouseData->booFollowAccel * deltaFrame;
                        if (this->speed > ghostHouseData->booFollowTopSpeed) {
                            this->speed = ghostHouseData->booFollowTopSpeed;
                        }
                        dirVec = (dirVec / dirVec.length())*this->speed*deltaFrame ;

                        Vec3f newPos = currentPos + dirVec;
                        this->setPos(&newPos);
                    }

                }
            }
            break;
    }

}

void grGhostHouseBoo::setVanish() {
    this->changeState(State_Vanish);
}

void grGhostHouseBoo::setSpawn(stRange* spawnRange, Vec3f* centerPos, bool useAltAnim) {
    stRange range = {spawnRange->m_left + centerPos->m_x, spawnRange->m_right + centerPos->m_x, spawnRange->m_top + centerPos->m_y, spawnRange->m_bottom + centerPos->m_y};
    this->setPos(randf()*(range.m_right - range.m_left) + range.m_left, randf()*(range.m_top - range.m_bottom) + range.m_bottom, 0);
    this->useAltAnim = useAltAnim;
    this->changeState(State_Spawn);
}

void grGhostHouseBoo::setStalk(int playerTarget) {
    this->playerTarget = playerTarget;
    this->changeState(grGhostHouseBoo::State_StalkStart);
}

void grGhostHouseBoo::setCircle(grGimmickMotionPath* motionPath, float startRatio, float circleSpeed) {
    this->m_gimmickMotionPath = motionPath;
    this->m_gimmickMotionPath->startMove();
    this->m_gimmickMotionPath->setFrame(motionPath->m_modelAnims[0]->getFrameCount()*startRatio);
    this->m_gimmickMotionPath->setFrameUpdate(0);
    this->speed = circleSpeed;
    this->changeState(State_CircleStart);
}

void grGhostHouseBoo::setCircle(Vec2f* circleCenterPos, float circleRadius, float circleCurrentAngle, float circleAngleSpeed) {
    this->circleCenterPos = *circleCenterPos;
    this->circleRadius = circleRadius;
    this->circleCurrentAngle = circleCurrentAngle;
    this->speed = circleAngleSpeed;
    this->changeState(State_CircleStart);
}

void grGhostHouseBoo::setSnakeLeader(stRange* spawnRange, Vec3f* centerPos) {
    stGhostHouseData* ghostHouseData = static_cast<stGhostHouseData*>(this->getStageData());

    u8 randomStartIndex = randi(4);
    float angle = mtConvRadToDeg(atan2(BOO_SNAKE_DIR_Y, BOO_SNAKE_DIR_X));
    switch (randomStartIndex) {
        case 0:
            this->direction = (Vec2f) {BOO_SNAKE_DIR_X, -BOO_SNAKE_DIR_Y};
            this->setRot(angle, ghostHouseData->booRot, 0);
            break;
        case 1:
            this->direction = (Vec2f) {-BOO_SNAKE_DIR_X, -BOO_SNAKE_DIR_Y};
            this->setRot(angle, -ghostHouseData->booRot, 0);
            break;
        case 2:
            this->direction = (Vec2f) {-BOO_SNAKE_DIR_X, BOO_SNAKE_DIR_Y};
            this->setRot(-angle, -ghostHouseData->booRot, 0);
            break;
        case 3:
            this->direction = (Vec2f) {BOO_SNAKE_DIR_X, BOO_SNAKE_DIR_Y};
            this->setRot(-angle, ghostHouseData->booRot, 0);
            break;
        default:
            break;
    }
    this->setPos(randf()*(spawnRange->m_right - spawnRange->m_left) + spawnRange->m_left + centerPos->m_x,
                 randf()*(spawnRange->m_top - spawnRange->m_bottom) + spawnRange->m_bottom + centerPos->m_y,
                 0.0);
    this->changeState(State_SnakeStart);
}

void grGhostHouseBoo::setSnakeFollow(grGhostHouseBoo* snakeLeader, float maxSnakeTimer, float snakeTimer) {
    this->snakeLeader = snakeLeader;
    this->maxSnakeTimer = maxSnakeTimer;
    this->timer = snakeTimer;
    this->changeState(State_SnakeStart);
}

void grGhostHouseBoo::setCrew(Vec2f* crewSWPos, Vec2f* crewNEPos) {
    this->southWestPos = crewSWPos;
    this->northEastPos = crewNEPos;

    stGhostHouseData* ghostHouseData = static_cast<stGhostHouseData*>(this->getStageData());

    this->setPos(randf()*(crewNEPos->m_x - crewSWPos->m_x) + crewSWPos->m_x, randf()*(crewNEPos->m_y - crewSWPos->m_y) + crewSWPos->m_y, 0.0);
    u8 randomStartIndex = randi(4);
    float angle = atan2(ghostHouseData->booCrewDirection.m_y, ghostHouseData->booCrewDirection.m_x);
    switch (randomStartIndex) {
        case 0:
            this->direction = (Vec2f) {ghostHouseData->booCrewDirection.m_x, -ghostHouseData->booCrewDirection.m_y};
            this->setRot(angle, ghostHouseData->booRot, 0);
            break;
        case 1:
            this->direction = (Vec2f) {-ghostHouseData->booCrewDirection.m_x, -ghostHouseData->booCrewDirection.m_y};
            this->setRot(angle, -ghostHouseData->booRot, 0);
            break;
        case 2:
            this->direction = (Vec2f) {-ghostHouseData->booCrewDirection.m_x, ghostHouseData->booCrewDirection.m_y};
            this->setRot(-angle, -ghostHouseData->booRot, 0);
            break;
        case 3:
            this->direction = (Vec2f) {ghostHouseData->booCrewDirection.m_x, ghostHouseData->booCrewDirection.m_y};
            this->setRot(-angle, ghostHouseData->booRot, 0);
            break;
        default:
            break;
    }
    this->remainingDistance = ghostHouseData->booCloudIdleMaxVerticalDistance;
    this->changeState(State_CrewStart);
}

void grGhostHouseBoo::setChase(Vec3f* startPos, Vec2f* targetPos) {
    stGhostHouseData* ghostHouseData = static_cast<stGhostHouseData*>(this->getStageData());

    this->prevPos = *startPos;
    this->targetPos = *targetPos;
    this->timer = ghostHouseData->booCrewChaseFramesToReach*2;
    this->changeState(State_ChaseStart);
}

void grGhostHouseBoo::setDisappear(Vec2f* disappearSWPos, Vec2f* disappearNEPos) {
    this->southWestPos = disappearSWPos;
    this->northEastPos = disappearNEPos;

    this->changeState(State_AppearStart);
}

void grGhostHouseBoo::changeState(State state) {
    stGhostHouseData* ghostHouseData = static_cast<stGhostHouseData*>(this->getStageData());

    if (this->state != state) {
        switch(state) {
            case State_Spawn:
                if (this->useAltAnim) {
                    this->setMotionDetails(6, 2, 0, 0, 2);
                }
                else {
                    this->setMotionDetails(5, 2, 0, 0, 2);
                }
                this->setSleepAttack(true);
                break;
            case State_Vanish:
                if (this->state != State_Defeat && this->state != State_Spawn && this->state != State_Disappear) {
                    if (this->state == State_Crew || this->state == State_CrewStart || this->state == State_ChaseFinish) {
                        if (this->useAltAnim) {
                            this->setMotionDetails(6, 2, 0, 0, 5);
                        }
                        else {
                            this->setMotionDetails(5, 2, 0, 0, 5);
                        }
                    }
                    else {
                        if (this->useAltAnim) {
                            this->setMotionDetails(6, 2, 0, 0, 2);
                        }
                        else {
                            this->setMotionDetails(5, 2, 0, 0, 2);
                        }
                    }

                    if (this->state == State_Stalk) {
                        Vec3f centerPos;
                        Vec3f bodyPos;
                        this->getNodePosition(&centerPos, 0, "center");
                        this->getNodePosition(&bodyPos, 0, "skl_root");

                        Vec3f pos = this->getPos();
                        Vec3f centerToBodyPos = bodyPos - centerPos;
                        this->setPos(pos.m_x + centerToBodyPos.m_x, pos.m_y + centerToBodyPos.m_y, pos.m_z);
                    }

                    this->setSleepAttack(true);
                    this->speed = 0;
                    if (this->m_gimmickMotionPath != NULL) {
                        this->m_gimmickMotionPath->setFrameUpdate(0);
                        this->m_gimmickMotionPath->clearFrame();
                        this->m_gimmickMotionPath->stopStartSE();
                    }
                    this->m_gimmickMotionPath = NULL;
                }
                break;
            case State_StalkStart:
                for (int i = 0; i < g_ftManager->getEntryCount(); i++) {
                    int entryId = g_ftManager->getEntryIdFromIndex(i);
                    if (this->playerTarget == g_ftManager->getPlayerNo(entryId) && g_ftManager->isFighterActivate(entryId, -1)) {
                        Vec3f targetPos = g_ftManager->getFighterCenterPos(entryId, -1);
                        float angle = randf()*2*M_PI;
                        this->setPos(targetPos.m_x + ghostHouseData->booFollowSpawnRadius*cosf(angle), targetPos.m_y + ghostHouseData->booFollowSpawnRadius*sinf(angle), 0);
                    }
                }
                this->setMotionDetails(1, 0, 0, 0, 9);
                this->setRot(0, 0, 0);
                this->prevFollowAnimFrame = 0;
                break;
            case State_Stalk:
                if (this->state != State_ShyStart) {
                    this->setSleepAttack(false);
                    this->setMotionDetails(0, 0, 0, 0, 0);
                    if (this->prevFollowAnimFrame != 0) {
                        this->m_modelAnims[0]->setFrame(this->prevFollowAnimFrame);
                        this->setPos(&this->prevPos);
                    }
                }
                break;
            case State_ShyStart:
            {
                if (this->state == State_ShyStart || this->state == State_Shy) {
                    return;
                }
                this->prevFollowAnimFrame = this->m_modelAnims[0]->getFrame();
                this->prevPos = this->getPos();

                this->speed = 0;
                Vec3f centerPos;
                Vec3f bodyPos;
                this->getNodePosition(&centerPos, 0, "center");
                this->getNodePosition(&bodyPos, 0, "skl_root");

                Vec3f centerToBodyPos = bodyPos - centerPos;
                this->setPos(this->prevPos.m_x + centerToBodyPos.m_x, this->prevPos.m_y + centerToBodyPos.m_y, this->prevPos.m_z);
                this->setMotionDetails(2, 3, 0, 0, 0);
            }

                break;
            case State_Shy:
                this->setMotionDetails(3, 3, 0, 0, 0);
                break;
            case State_CircleStart:
                if (this->useAltAnim) {
                    this->setMotionDetails(6, 0, 0, 0, 1);
                }
                else {
                    this->setMotionDetails(5, 0, 0, 0, 1);
                }
                if (this->m_gimmickMotionPath == NULL) {
                    this->setPos(this->circleCenterPos.m_x + this->circleRadius*cos(this->circleCurrentAngle), this->circleCenterPos.m_y + this->circleRadius*sin(this->circleCurrentAngle), 0);
                }
                break;
            case State_Circle:
                if (this->useAltAnim) {
                    this->setMotionDetails(6, 0, 0, 0, 0);
                }
                else {
                    this->setMotionDetails(5, 0, 0, 0, 0);
                }
                this->setSleepAttack(false);
                if (this->m_gimmickMotionPath != NULL) {
                    this->m_gimmickMotionPath->setFrameUpdate(this->speed);
                }
                break;
            case State_SnakeStart:
                if (this->useAltAnim) {
                    this->setMotionDetails(6, 0, 0, 0, 1);
                }
                else {
                    this->setMotionDetails(5, 0, 0, 0, 1);
                }
                if (this->snakeLeader != NULL) {
                    Vec3f leaderPos = this->snakeLeader->getPos();
                    this->setPos(&leaderPos);
                    Vec3f leaderRot = this->snakeLeader->getRot();
                    this->setRot(&leaderRot);
                }
                break;
            case State_Snake:
                if (this->useAltAnim) {
                    this->setMotionDetails(6, 0, 0, 0, 0);
                }
                else {
                    this->setMotionDetails(5, 0, 0, 0, 0);
                }
                this->setSleepAttack(false);
                break;
            case State_CrewStart:
                this->speed = ghostHouseData->booCrewIdleTopSpeed;
                this->accel = ghostHouseData->booCrewIdleAccel;
                if (this->useAltAnim) {
                    this->setMotionDetails(6, 0, 0, 0, 10);
                }
                else {
                    this->setMotionDetails(5, 0, 0, 0, 4);
                }
                break;
            case State_Crew:
                if (this->useAltAnim) {
                    this->setMotionDetails(6, 0, 0, 0, 3);
                }
                else {
                    this->setMotionDetails(5, 0, 0, 0, 3);
                }
                break;
            case State_ChaseStart:
                if (this->useAltAnim) {
                    this->setMotionDetails(6, 0, 0, 0, 11);
                }
                else {
                    this->setMotionDetails(5, 0, 0, 0, 6);
                }
                break;
            case State_Chase:
                if (this->useAltAnim) {
                    this->setMotionDetails(6, 0, 0, 0, 0);
                }
                else {
                    this->setMotionDetails(5, 0, 0, 0, 0);
                }
                this->setSleepAttack(false);
                break;
            case State_ChaseFinish:
                if (this->useAltAnim) {
                    this->setMotionDetails(6, 0, 0, 0, 12);
                }
                else {
                    this->setMotionDetails(5, 0, 0, 0, 7);
                }

                this->setSleepAttack(true);
                break;
            case State_AppearStart:
                if (this->useAltAnim) {
                    this->setMotionDetails(6, 0, 0, 0, 13);
                }
                else {
                    this->setMotionDetails(5, 0, 0, 0, 8);
                }
                this->setPos(randf()*(this->northEastPos->m_x - this->southWestPos->m_x) + this->southWestPos->m_x,
                             randf()*(this->northEastPos->m_y - this->southWestPos->m_y) + this->southWestPos->m_y,
                             0.0);
                this->timer = ghostHouseData->booDisappearingAppearFrames;
                break;
            case State_Appear:
                if (this->useAltAnim) {
                    this->setMotionDetails(6, 0, 0, 0, 0);
                }
                else {
                    this->setMotionDetails(5, 0, 0, 0, 0);
                }
                this->setSleepAttack(false);
                break;
            case State_Disappear:
                if (this->useAltAnim) {
                    this->setMotionDetails(6, 2, 0, 0, 2);
                }
                else {
                    this->setMotionDetails(5, 2, 0, 0, 2);
                }
                this->setSleepAttack(true);
                this->timer = ghostHouseData->booDisappearingDisappearFrames;
                break;
            default:
                break;
        }
        this->state = state;

    }
}

bool grGhostHouseBoo::findClosestFighterDisp(Vec3f* outDisp) {
    Vec3f pos = this->getPos();
    *outDisp = (Vec3f){0, 0, 0};
    float closestDist = HUGE_VALF;
    for (int i = 0; i < g_ftManager->getEntryCount(); i++) {
        int entryId = g_ftManager->getEntryIdFromIndex(i);
        if (g_ftManager->isFighterActivate(entryId, -1)) {
            Vec3f disp = g_ftManager->getFighterCenterPos(entryId, -1) - pos;
            float dist = disp.length();
            if (closestDist > dist) {
                closestDist = dist;
                *outDisp = disp;
            };
        }
    }
    return closestDist < HUGE_VALF;
}

void grGhostHouseBoo::rotateToDisp(Vec2f* disp, float maxRot, float rotateSpeed) {
    Vec3f currentRot = this->getRot();
    float pitch = mtConvRadToDeg(hkMath::acos(fabsf(disp->m_y)/disp->length()));
    pitch = hkMath::min2(maxRot, pitch);
    if (disp->m_x < 0) {
        pitch = -pitch;
    }
    float pitchDiff = pitch - currentRot.m_pitch;
    if (pitchDiff >= 0) {
        pitchDiff = hkMath::min2(pitchDiff, rotateSpeed);
    }
    else {
        pitchDiff = hkMath::max2(pitchDiff, -rotateSpeed);
    }
    currentRot.m_pitch += pitchDiff;

    float roll = mtConvRadToDeg(hkMath::acos(fabsf(disp->m_x)/disp->length()));
    roll = hkMath::min2(maxRot, roll);
    if (disp->m_y >= 0) {
        roll = -roll;
    }
    float rollDiff = roll - currentRot.m_roll;
    if (rollDiff >= 0) {
        rollDiff = hkMath::min2(rollDiff, rotateSpeed);
    }
    else {
        rollDiff = hkMath::max2(rollDiff, -rotateSpeed);
    }
    currentRot.m_roll += rollDiff;

    this->setRot(&currentRot);
}