#include "gr_ghosthouse_bubble.h"
#include <ec/ec_mgr.h>
#include <memory.h>
#include <mt/mt_prng.h>
#include <math.h>
#include <OS/OSError.h>

grGhostHouseBubble* grGhostHouseBubble::create(int mdlIndex, const char* tgtNodeName, const char* taskName)
{
    grGhostHouseBubble* bubble = new (Heaps::StageInstance) grGhostHouseBubble(taskName);
    bubble->setMdlIndex(mdlIndex);
    bubble->m_heapType = Heaps::StageInstance;
    bubble->makeCalcuCallback(1, Heaps::StageInstance);
    bubble->setCalcuCallbackRoot(7);
    bubble->setupMelee();

    return bubble;
}

void grGhostHouseBubble::setupAttack() {
    stGhostHouseData* ghostHouseData = static_cast<stGhostHouseData*>(this->getStageData());

    float size = 1.0;
    Vec3f offsetPos = Vec3f(0.0, 0.0, 0.0);
    this->setAttack(size, &offsetPos);
    this->m_attackInfo->m_preset = 4;

    soCollisionAttackData* overwriteAttackData = this->getOverwriteAttackData();
    this->createAttackPointNormal(overwriteAttackData);
    overwriteAttackData->m_reactionEffect = ghostHouseData->bubbleReactionEffect;
    overwriteAttackData->m_reactionFix = ghostHouseData->bubbleReactionFix;
    overwriteAttackData->m_reactionAdd = ghostHouseData->bubbleReactionAdd;
    overwriteAttackData->m_power = ghostHouseData->bubblePower;
    overwriteAttackData->m_vector = ghostHouseData->bubbleVector;
    overwriteAttackData->m_size = 1.0;
    overwriteAttackData->m_offsetPos = offsetPos;
    overwriteAttackData->m_hitStopFrame = ghostHouseData->bubbleHitstopMultiplier;

    overwriteAttackData->m_nodeIndex = 0x3;

    overwriteAttackData->m_targetCategory = COLLISION_CATEGORY_MASK_ALL;

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

    overwriteAttackData->m_serialHitFrame = ghostHouseData->bubbleDetectionRate;
    overwriteAttackData->m_isDirect = false;
    overwriteAttackData->m_isInvalidInvincible = false;
    overwriteAttackData->m_isInvalidXlu = false;
    overwriteAttackData->m_lrCheck = soCollisionAttackData::Lr_Check_Pos;
    overwriteAttackData->m_isCatch = false;
    overwriteAttackData->m_noTeam = false;
    overwriteAttackData->m_noHitStop = false;
    overwriteAttackData->m_noEffect = false;
    overwriteAttackData->m_noTransaction = false;
    overwriteAttackData->m_subShield = ghostHouseData->bubbleShieldDamage;

    overwriteAttackData->m_shapeType = soCollision::Shape_Capsule;
}

void grGhostHouseBubble::update(float deltaFrame) {
    grMadein::update(deltaFrame);
    this->updateMove(deltaFrame);
}

void grGhostHouseBubble::updateMove(float deltaFrame) {
    stGhostHouseData *ghostHouseData = static_cast<stGhostHouseData*>(this->getStageData());
    switch(this->state) {
        case State_Out:
            this->timer -= deltaFrame;
            if (this->timer <= 0) {
                this->changeState(State_In);
            }
            break;
        case State_Exit:
        case State_In:
        {
            Vec3f pos = this->getPos();
            this->setPos(pos.m_x + ghostHouseData->bubbleSpeedX*deltaFrame*this->directionX,
                         ghostHouseData->bubbleAmplitude*sin((2*M_PI/ghostHouseData->bubblePeriod)*this->timer) + this->startPosY,
                         0.0);

            this->timer += deltaFrame;
            if (this->directionX > 0) {
                if (pos.m_x >= this->northEastPos->m_x) {
                    this->changeState(State_Out);
                }
            }
            else {
                if (pos.m_x <= this->southWestPos->m_x) {
                    this->changeState(State_Out);
                }
            }

        }
            break;
        default:
            break;
    }
}

void grGhostHouseBubble::setVanish() {
    if (this->state == State_In) {
        this->changeState(State_Exit);
    }
    else {
        this->changeState(State_Vanish);
    }
}

void grGhostHouseBubble::setActive(Vec2f* boundarySWPos, Vec2f* boundaryNEPos) {
    this->southWestPos = boundarySWPos;
    this->northEastPos = boundaryNEPos;

    this->changeState(State_Out);
}

void grGhostHouseBubble::changeState(State state) {
    stGhostHouseData *ghostHouseData = static_cast<stGhostHouseData *>(this->getStageData());
    if (this->state != state) {
        switch (state) {
            case State_Out:
                if (this->state == State_Exit) {
                    state = State_Vanish;
                }
                this->timer = randi(ghostHouseData->bubbleRespawnMaxFrames - ghostHouseData->bubbleRespawnMinFrames) + ghostHouseData->bubbleRespawnMinFrames;
            case State_Vanish:
                this->setNodeVisibilityAll(false, 0);
                this->setSleepAttack(true);
                break;
            case State_In:
                this->timer = randf() * ghostHouseData->bubblePeriod;
                this->directionX = -1.0f + 2 * randi(2);
                this->startPosY = randf()*(this->northEastPos->m_y - this->southWestPos->m_y) + this->southWestPos->m_y;
                if (this->directionX > 0) {
                    this->setPos(this->southWestPos->m_x, this->startPosY, 0.0);
                }
                else {
                    this->setPos(this->northEastPos->m_x, this->startPosY, 0.0);
                }
                this->setNodeVisibilityAll(true, 0);
                this->setSleepAttack(false);
                this->setMotion(0);
                break;
            default:
                break;
        }
        this->state = state;
    }
}

