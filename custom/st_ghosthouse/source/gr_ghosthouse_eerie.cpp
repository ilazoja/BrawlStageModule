#include "gr_ghosthouse_eerie.h"
#include <ec/ec_mgr.h>
#include <memory.h>
#include <mt/mt_prng.h>
#include <math.h>
#include <OS/OSError.h>

grGhostHouseEerie* grGhostHouseEerie::create(int mdlIndex, const char* tgtNodeName, const char* taskName)
{
    grGhostHouseEerie* eerie = new (Heaps::StageInstance) grGhostHouseEerie(taskName);
    eerie->setMdlIndex(mdlIndex);
    eerie->m_heapType = Heaps::StageInstance;
    eerie->makeCalcuCallback(1, Heaps::StageInstance);
    eerie->setCalcuCallbackRoot(7);
    eerie->setupMelee();

    return eerie;
}

void grGhostHouseEerie::setupAttack() {
    stGhostHouseData* ghostHouseData = static_cast<stGhostHouseData*>(this->getStageData());

    float size = 1.0;
    Vec3f offsetPos = Vec3f(0.0, 0.0, 0.0);
    this->setAttack(size, &offsetPos);
    this->m_attackInfo->m_preset = Attack_Overwrite;

    soCollisionAttackData* overwriteAttackData = this->getOverwriteAttackData();
    this->createAttackPointNormal(overwriteAttackData);
    overwriteAttackData->m_reactionEffect = ghostHouseData->eerieReactionEffect;
    overwriteAttackData->m_reactionFix = ghostHouseData->eerieReactionFix;
    overwriteAttackData->m_reactionAdd = ghostHouseData->eerieReactionAdd;
    overwriteAttackData->m_power = ghostHouseData->eeriePower;
    overwriteAttackData->m_vector = ghostHouseData->eerieVector;
    overwriteAttackData->m_size = 1.0;
    overwriteAttackData->m_offsetPos = offsetPos;
    overwriteAttackData->m_hitStopFrame = ghostHouseData->eerieHitstopMultiplier;

    overwriteAttackData->m_nodeIndex = 23;

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

    overwriteAttackData->m_serialHitFrame = ghostHouseData->eerieDetectionRate;
    overwriteAttackData->m_isDirect= false;
    overwriteAttackData->m_isInvalidInvincible = false;
    overwriteAttackData->m_isInvalidXlu = false;
    overwriteAttackData->m_lrCheck = soCollisionAttackData::Lr_Check_Pos;
    overwriteAttackData->m_isCatch = false;
    overwriteAttackData->m_noTeam = false;
    overwriteAttackData->m_noHitStop = false;
    overwriteAttackData->m_noEffect = false;
    overwriteAttackData->m_noTransaction = false;
    overwriteAttackData->m_subShield= ghostHouseData->eerieShieldDamage;

    overwriteAttackData->m_shapeType = soCollision::Shape_Capsule;
}

void grGhostHouseEerie::update(float deltaFrame) {
    grMadein::update(deltaFrame);
    this->updateMove(deltaFrame);
}

void grGhostHouseEerie::updateMove(float deltaFrame) {
    stGhostHouseData *ghostHouseData = static_cast<stGhostHouseData*>(this->getStageData());
    switch(this->state) {
        case State_In:
        {
            Vec3f pos = this->getPos();
            this->setPos(pos.m_x + ghostHouseData->eerieSpeedX*deltaFrame*this->directionX,
                         this->amplitude*sin((2*M_PI/this->m_modelAnims[0]->getFrameCount())*(this->m_modelAnims[0]->getFrame() + EERIE_PHASE_SHIFT)) + this->startPosY,
                         0.0);

            if (this->directionX > 0) {
                if (pos.m_x >= this->northEastPos->m_x) {
                    this->changeState(State_Vanish);
                }
            }
            else {
                if (pos.m_x <= this->southWestPos->m_x) {
                    this->changeState(State_Vanish);
                }
            }

        }
            break;
    }

}

void grGhostHouseEerie::setVanish() {
    this->changeState(State_Vanish);
}

void grGhostHouseEerie::setActive(Vec2f* boundarySWPos, Vec2f* boundaryNEPos, Vec3f* startPos, float directionX, float amplitude, float startAnimFrame) {

    stGhostHouseData *ghostHouseData = static_cast<stGhostHouseData *>(this->getStageData());

    this->southWestPos = boundarySWPos;
    this->northEastPos = boundaryNEPos;
    this->amplitude = amplitude;
    this->changeState(State_In);
    this->m_modelAnims[0]->setFrame(startAnimFrame);
    this->startPosY = startPos->m_y;
    this->directionX = directionX;
    this->setPos(startPos);
    this->setRot(0, this->directionX*ghostHouseData->eerieRot, 0);
}

void grGhostHouseEerie::changeState(State state) {
    stGhostHouseData *ghostHouseData = static_cast<stGhostHouseData *>(this->getStageData());
    if (this->state != state) {
        switch (state) {
            case State_Vanish:
                this->setNodeVisibilityAll(false, 0);
                this->setSleepAttack(true);
                this->setMotion(0);
                this->setMotionRatio(ghostHouseData->eerieAnimRatio);
                break;
            case State_In:
                this->setNodeVisibilityAll(true, 0);
                this->setSleepAttack(false);
                break;
            default:
                break;
        }
        this->state = state;
    }
}

bool grGhostHouseEerie::isAvailable() {
    return this->state == State_Vanish;
}

float grGhostHouseEerie::getPeriod() {
    return this->m_modelAnims[0]->getFrameCount()*this->m_motionRatio;
}

