#include "gr_ghosthouse_bigboo.h"
#include <ec/ec_mgr.h>
#include <memory.h>
#include <mt/mt_prng.h>
#include <mt/mt_trig.h>
#include <math.h>
#include <OS/OSError.h>
#include <ft/ft_manager.h>

grGhostHouseBigBoo* grGhostHouseBigBoo::create(int mdlIndex, const char* tgtNodeName, const char* taskName)
{
    grGhostHouseBigBoo* boo = new (Heaps::StageInstance) grGhostHouseBigBoo(taskName);
    boo->setMdlIndex(mdlIndex);
    boo->m_heapType = Heaps::StageInstance;
    boo->makeCalcuCallback(1, Heaps::StageInstance);
    boo->setCalcuCallbackRoot(7);
    boo->setupMelee();

    return boo;
}

// TODO: Does not target the person who hit him last

void grGhostHouseBigBoo::startup(gfArchive* archive, u32 unk1, gfSceneRoot::LayerType layerType) {
    grMadein::startup(archive, unk1, layerType);

    this->createSoundWork(1,1);
    this->m_soundEffects[0].m_id = snd_se_stage_Madein_01;
    this->m_soundEffects[0].m_repeatFrame = 0;
    this->m_soundEffects[0].m_nodeIndex = 0;
    this->m_soundEffects[0].m_endFrame = 0;
    this->m_soundEffects[0].m_offsetPos = Vec2f(0.0, 0.0);

    this->setupAttack();
    this->setupHitPoint();

    this->initializeEntity();
    this->startEntity();

    this->setHit();
}

void grGhostHouseBigBoo::update(float deltaFrame)
{
    grMadein::update(deltaFrame);
    this->updateMove(deltaFrame);
}

void grGhostHouseBigBoo::setupAttack() {
    stGhostHouseData* ghostHouseData = static_cast<stGhostHouseData*>(this->getStageData());

    float size = 1.0;
    Vec3f offsetPos = Vec3f(0.0, 0.0, 0.0);
    this->setAttack(size, &offsetPos);
    this->m_attackInfo->m_preset = Attack_Overwrite;

    soCollisionAttackData* overwriteAttackData = this->getOverwriteAttackData();
    this->createAttackPointNormal(overwriteAttackData);
    overwriteAttackData->m_reactionEffect = ghostHouseData->bigBooReactionEffect;
    overwriteAttackData->m_reactionFix = ghostHouseData->bigBooReactionFix;
    overwriteAttackData->m_reactionAdd = ghostHouseData->bigBooReactionAdd;
    overwriteAttackData->m_power = ghostHouseData->bigBooPower;
    overwriteAttackData->m_vector = ghostHouseData->bigBooVector;
    overwriteAttackData->m_size = 1.0;
    overwriteAttackData->m_offsetPos = offsetPos;
    overwriteAttackData->m_hitStopFrame = ghostHouseData->bigBooHitstopMultiplier;

    overwriteAttackData->m_nodeIndex = this->getNodeIndex(0, "Hurt");

    overwriteAttackData->m_targetCategory = COLLISION_CATEGORY_MASK_NO_ITEM;

    overwriteAttackData->m_targetSituationODD = true;
    overwriteAttackData->m_targetSituationAir = true;
    overwriteAttackData->m_targetSituationGround = true;

    overwriteAttackData->m_targetLr = false;
    overwriteAttackData->m_targetPart = COLLISION_PART_MASK_ALL;
    overwriteAttackData->m_attribute = soCollisionAttackData::Attribute_Normal;

    overwriteAttackData->m_soundLevel = soCollisionAttackData::Sound_Level_Small;
    overwriteAttackData->m_soundAttribute = soCollisionAttackData::Sound_Attribute_Punch;
    overwriteAttackData->m_setOffKind = soCollisionAttackData::SetOff_Off;

    overwriteAttackData->m_noScale = false;
    overwriteAttackData->m_isShieldable = true;
    overwriteAttackData->m_isReflectable = false;
    overwriteAttackData->m_isAbsorbable = false;
    overwriteAttackData->m_region = soCollisionAttackData::Region_None;

    overwriteAttackData->m_serialHitFrame = ghostHouseData->bigBooDetectionRate;
    overwriteAttackData->m_isDirect = false;
    overwriteAttackData->m_isInvalidInvincible = false;
    overwriteAttackData->m_isInvalidXlu = false;
    overwriteAttackData->m_lrCheck = soCollisionAttackData::Lr_Check_Pos;
    overwriteAttackData->m_isCatch = false;
    overwriteAttackData->m_noTeam = false;
    overwriteAttackData->m_noHitStop = false;
    overwriteAttackData->m_noEffect = false;
    overwriteAttackData->m_noTransaction = false;
    overwriteAttackData->m_subShield = ghostHouseData->bigBooShieldDamage;

    overwriteAttackData->m_shapeType = soCollision::Shape_Capsule;
}

void grGhostHouseBigBoo::setupHitPoint() {
    Vec3f startOffsetPos = Vec3f(0,0,0);
    Vec3f endOffsetPos = Vec3f(0,0,0);
    this->setHitPoint(1.0, &startOffsetPos, &endOffsetPos, 1, this->getNodeIndex(0, "Hurt"));
}

void grGhostHouseBigBoo::setHit() {
    // Done cause Brawl devs allocated ykData on the stack in grMadein::setupYakumonoClass leading to ykData being a garbage pointer so have to replace it in order to be able to change the HitSelfCategory
    this->m_yakumono->m_data = &this->yakumonoData;
    this->setSituationODD();
}

void grGhostHouseBigBoo::setSpawn() {
    this->changeState(State_Spawn);
}

void grGhostHouseBigBoo::setVanish() {
    this->changeState(State_Vanish);
}

void grGhostHouseBigBoo::setStalk(Vec2f* bigBooSWPos, Vec2f* bigBooNEPos) {
    this->southWestPos = bigBooSWPos;
    this->northEastPos = bigBooNEPos;

    Vec3f pos = this->getRandPos();
    this->setPos(&pos);

    this->changeState(State_StalkStart);
}

void grGhostHouseBigBoo::onDamage(int index, soDamage* damage, soDamageAttackerInfo* attackerInfo) {
    stGhostHouseData* ghostHouseData = static_cast<stGhostHouseData*>(this->getStageData());

    this->numHits++;

    if (this->numHits >= ghostHouseData->bigBooNumHitsToDefeat) {
        this->changeState(State_Defeat);
    }
    else {
        this->changeState(State_HideStart);
    }

}

void grGhostHouseBigBoo::updateMove(float deltaFrame) {
    stGhostHouseData* ghostHouseData = static_cast<stGhostHouseData*>(this->getStageData());
    float currentAnimFrame = this->m_modelAnims[0]->getFrame();
    float animFrameCount = this->m_modelAnims[0]->getFrameCount();
    switch(this->state) {
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
        case State_Stalk: {
            Vec3f pos = this->getPos();
            Vec3f dirVec = Vec3f(0, 0, 0);
            float closestDist = HUGE_VALF;
            float closestFighterLr = 1.0;

            for (int i = 0; i < g_ftManager->getEntryCount(); i++) {
                int entryId = g_ftManager->getEntryIdFromIndex(i);
                if (g_ftManager->isFighterActivate(entryId, -1)) {
                    Vec3f disp = g_ftManager->getFighterCenterPos(entryId, -1) - pos;
                    float dist = disp.length();
                    if (closestDist > dist) {
                        closestDist = dist;
                        dirVec = disp;
                        closestFighterLr = g_ftManager->getFighterLr(entryId, -1);
                    };
                }
            }

            if (closestDist < HUGE_VALF) {
                Vec2f rotDir;
                if (dirVec.m_x >= 0) {
                    rotDir = Vec2f(1.0, 0.0);
                    if (closestFighterLr < 0) {
                        this->changeState(State_ShyStart);
                    } else {
                        this->changeState(State_Stalk);
                    }
                } else {
                    rotDir = Vec2f(-1.0, 0.0);

                    if (closestFighterLr > 0) {
                        this->changeState(State_ShyStart);
                    } else {
                        this->changeState(State_Stalk);
                    }
                }
                this->rotateToDisp(&rotDir, ghostHouseData->booRot, deltaFrame * BOO_ROT_SPEED);
                if (this->state == State_Stalk) {
                    Vec3f currentPos = this->getPos();
                    this->speed += ghostHouseData->bigBooFollowAccel * deltaFrame;
                    if (this->speed > ghostHouseData->bigBooFollowTopSpeed) {
                        this->speed = ghostHouseData->bigBooFollowTopSpeed;
                    }
                    dirVec = (dirVec / dirVec.length()) * this->speed * deltaFrame;

                    Vec3f newPos = currentPos + dirVec;
                    this->setPos(&newPos);
                }

            }
        }
            break;
        case State_HideStart:
            if (currentAnimFrame >= animFrameCount - 1) {
                this->changeState(State_Hide);
            }
            break;
        case State_Hide: {
            Vec3f currentPos = this->getPos();
            Vec3f dirVec = this->hideDestPos - currentPos;
            if (dirVec.length() <= ghostHouseData->bigBooFollowTopSpeed) {
                this->hideDestPos = this->getRandPos();
            }

            Vec2f rotDir;
            if (dirVec.m_x >= 0) {
                rotDir = Vec2f(1.0, 0.0);
            } else {
                rotDir = Vec2f(-1.0, 0.0);
            }

            this->rotateToDisp(&rotDir, ghostHouseData->booRot, deltaFrame * BOO_ROT_SPEED);
            this->speed += ghostHouseData->bigBooFollowAccel * deltaFrame;
            if (this->speed > ghostHouseData->bigBooFollowTopSpeed) {
                this->speed = ghostHouseData->bigBooFollowTopSpeed;
            }
            dirVec = (dirVec / dirVec.length()) * this->speed * deltaFrame;

            Vec3f newPos = currentPos + dirVec;
            this->setPos(&newPos);

            this->timer -= deltaFrame;
            if (this->timer <= 0) {
                this->changeState(State_HideEnd);
            }
        }
            break;
        case State_HideEnd:
            if (currentAnimFrame >= animFrameCount - 1) {
                this->changeState(State_Stalk);
            }
            break;
        default:
            break;
    }
}

void grGhostHouseBigBoo::changeState(State state) {
    stGhostHouseData *ghostHouseData = static_cast<stGhostHouseData *>(this->getStageData());

    if (this->state != state) {
        switch (state) {
            case State_Spawn:
                this->setMotionDetails(3, 5, 0, 0, 0);
                this->setSleepAttack(true);
                this->setSleepHit(true);
                break;
            case State_Vanish:
                if (this->state != State_Defeat && this->state != State_Spawn) {
                    this->setMotionDetails(5, 2, 0, 0, 2);
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
                this->setSleepHit(true);
                this->speed = 0;

                break;
            case State_StalkStart:
                this->setMotionDetails(1, 0, 0, 0, 9);
                this->setRot(0, 0, 0);
                this->prevFollowAnimFrame = 0;
                this->numHits = 0;

                break;
            case State_Stalk:
                if (this->state != State_ShyStart) {
                    this->setSleepAttack(false);
                    this->setSleepHit(false);
                    this->setMotionDetails(0, 0, 0, 0, 0); // TODO: Change based on whether angered or not
                    if (this->prevFollowAnimFrame != 0) {
                        this->m_modelAnims[0]->setFrame(this->prevFollowAnimFrame);
                        this->setPos(&this->prevPos);
                    }
                }
                break;
            case State_ShyStart:
            {
                if (this->state == State_ShyStart || this->state == State_Shy || this->numHits >= ghostHouseData->bigBooNumHitsToAnger) {
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
            case State_HideStart:
                this->setSleepAttack(true);
                this->setSleepHit(true);
                this->timer = ghostHouseData->bigBooHitCooldown;
                this->speed = 0;
                this->hideDestPos = this->getRandPos();
                this->prevFollowAnimFrame = 0;
                this->setMotionDetails(3, 3, 0, 0, 7);
                break;
            case State_Hide:
                this->setMotionDetails(3, 3, 0, 0, 3);
                break;
            case State_HideEnd:
                this->setMotionDetails(3, 3, 0, 0, 6);
                break;
            case State_Defeat:
                this->setMotionDetails(4, 2, 0, 0, 2);
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
                this->setSleepHit(true);
                this->speed = 0;
                break;
            default:
                break;
        }
        this->state = state;
    }
}

void grGhostHouseBigBoo::rotateToDisp(Vec2f* disp, float maxRot, float rotateSpeed) {
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

Vec3f grGhostHouseBigBoo::getRandPos() {
    return Vec3f(randf()*(this->northEastPos->m_x - this->southWestPos->m_x) + this->southWestPos->m_x, randf()*(this->northEastPos->m_y - this->southWestPos->m_y) + this->southWestPos->m_y, 0.0);
}