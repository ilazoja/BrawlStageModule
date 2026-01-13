#include <memory.h>
#include <ec/ec_mgr.h>
#include "gr_bowsercastle_thwomp.h"
#include <mt/mt_prng.h>
#include <cm/cm_quake.h>
#include <cm/cm_subject.h>
#include <gm/gm_global.h>
#include <ai/ai_mgr.h>
#include <OS/OSError.h>

grBowserCastleThwomp* grBowserCastleThwomp::create(int mdlIndex, const char* tgtNodeName, const char* taskName){
    grBowserCastleThwomp* thwomp = new(Heaps::StageInstance) grBowserCastleThwomp(taskName);
    thwomp->setupMelee();
    thwomp->setMdlIndex(mdlIndex);
    thwomp->m_heapType = Heaps::StageInstance;
    thwomp->makeCalcuCallback(1, Heaps::StageInstance);
    thwomp->setCalcuCallbackRoot(7);

    return thwomp;
}

void grBowserCastleThwomp::startup(gfArchive* archive, u32 unk1, gfSceneRoot::LayerType layerType) {
    grMadein::startup(archive, unk1, layerType);

    stBowserCastleData* stageData = static_cast<stBowserCastleData*>(this->getStageData());

    this->createEffectWork(2);
    SimpleEffectData simpleEffectData;
    this->createSimpleEffectData(&simpleEffectData, (EfID)0x320003, "EffectShake");
    if (0 < simpleEffectData.m_id) {
        this->m_effects[0].m_id = simpleEffectData.m_id;
        this->m_effects[0].m_repeatFrame = simpleEffectData.m_repeatFrame;
        this->m_effects[0].m_nodeIndex = simpleEffectData.m_nodeIndex;
        this->m_effects[0].m_endFrame = simpleEffectData.m_endFrame;
        this->m_effects[0].m_offsetPos = Vec2f(0.0, 0.0);
        this->m_effects[0].m_scale = 1.0;
    }
    this->createSimpleEffectData(&simpleEffectData, (EfID)0x320004, "EffectDrop");
    if (0 < simpleEffectData.m_id) {
        this->m_effects[1].m_id = simpleEffectData.m_id;
        this->m_effects[1].m_repeatFrame = simpleEffectData.m_repeatFrame;
        this->m_effects[1].m_nodeIndex = simpleEffectData.m_nodeIndex;
        this->m_effects[1].m_endFrame = simpleEffectData.m_endFrame;
        this->m_effects[1].m_offsetPos = Vec2f(0.0, 0.0);
        this->m_effects[1].m_scale = 1.0;
    }

    this->createSoundWork(4,4);
    this->m_soundEffects[0].m_id = snd_se_stage_BowserCastle_Thwomp_Spawn;
    this->m_soundEffects[0].m_repeatFrame = 0;
    this->m_soundEffects[0].m_nodeIndex = simpleEffectData.m_nodeIndex;
    this->m_soundEffects[0].m_endFrame = 0;
    this->m_soundEffects[0].m_offsetPos = Vec2f(0.0, 0.0);
    this->m_soundEffects[1].m_id = snd_se_stage_BowserCastle_Thwomp_Shake;
    this->m_soundEffects[1].m_repeatFrame = 0;
    this->m_soundEffects[1].m_nodeIndex = simpleEffectData.m_nodeIndex;
    this->m_soundEffects[1].m_endFrame = 0;
    this->m_soundEffects[1].m_offsetPos = Vec2f(0.0, 0.0);
    this->m_soundEffects[2].m_id = snd_se_stage_BowserCastle_Thwomp_Land;
    this->m_soundEffects[2].m_repeatFrame = 0;
    this->m_soundEffects[2].m_nodeIndex = simpleEffectData.m_nodeIndex;
    this->m_soundEffects[2].m_endFrame = 0;
    this->m_soundEffects[2].m_offsetPos = Vec2f(0.0, 0.0);
    this->m_soundEffects[3].m_id = snd_se_stage_BowserCastle_Thwomp_Stomp;
    this->m_soundEffects[3].m_repeatFrame = 0;
    this->m_soundEffects[3].m_nodeIndex = simpleEffectData.m_nodeIndex;
    this->m_soundEffects[3].m_endFrame = 0;
    this->m_soundEffects[3].m_offsetPos = Vec2f(0.0, 0.0);

    Vec3f areaPosSW;
    Vec3f areaPosNE;
    this->getNodePosition(&areaPosSW, 0, "AreaSW");
    this->getNodePosition(&areaPosNE, 0, "AreaNE");
    this->areaData.set(gfArea::Shape_Rectangle, gfArea::Stage_Group_Gimmick_Normal, 0, 0, 0, 0, *(areaPosSW + areaPosNE).xy() / 2, *(areaPosSW - areaPosNE).xy());

    this->setupAttack();
    this->setAreaGimmick(&this->areaData, &this->areaInit, &this->areaInfo, true);
    stTrigger* trigger = g_stTriggerMng->createTrigger(Gimmick::Area_Common,-1);
    trigger->setObserveYakumono(this->m_yakumono);
    this->startEntity();

    this->state = State_Inactive;
    this->setSleepArea(true);
    this->setMotion(State_Stomp);
    this->setMotionFrame(this->m_modelAnims[0]->getFrameCount(), 0);
    this->timer = randi(stageData->thwompMaxRespawnFrames - stageData->thwompMinRespawnFrames) + stageData->thwompMinRespawnFrames;
}

void grBowserCastleThwomp::setShadow(grBowserCastleShadow* shadow) {
    this->shadow = shadow;
}

void grBowserCastleThwomp::setType(u8 type) {
    this->type = (Type)type;
}

void grBowserCastleThwomp::setupAttack() {

    float size = 1.0;
    Vec3f offsetPos = Vec3f(0.0, 100.0, 0.0);
    this->setAttack(size, &offsetPos);
    this->m_attackInfo->m_preset = Attack_Overwrite;

    soCollisionAttackData* overwriteAttackData = this->getOverwriteAttackData();
    this->createAttackPointNormal(overwriteAttackData);
    overwriteAttackData->m_reactionEffect = 0;
    overwriteAttackData->m_reactionFix = 0;
    overwriteAttackData->m_reactionAdd = 0;
    overwriteAttackData->m_power = 1;
    overwriteAttackData->m_vector = 361;
    overwriteAttackData->m_size = 1.0;
    overwriteAttackData->m_offsetPos = offsetPos;
    overwriteAttackData->m_hitStopFrame = 0;

    overwriteAttackData->m_nodeIndex = this->getNodeIndex(0, "Attack");

    overwriteAttackData->m_targetCategory = COLLISION_CATEGORY_MASK_GIMMICK;

    overwriteAttackData->m_targetSituationODD = true;
    overwriteAttackData->m_targetSituationAir = false;
    overwriteAttackData->m_targetSituationGround = false;

    overwriteAttackData->m_targetLr = false;
    overwriteAttackData->m_targetPart = COLLISION_PART_MASK_ALL;
    overwriteAttackData->m_attribute = soCollisionAttackData::Attribute_None;

    overwriteAttackData->m_soundLevel = soCollisionAttackData::Sound_Level_Small;
    overwriteAttackData->m_soundAttribute = soCollisionAttackData::Sound_Attribute_None;
    overwriteAttackData->m_setOffKind = soCollisionAttackData::SetOff_Off;

    overwriteAttackData->m_noScale = false;
    overwriteAttackData->m_isShieldable = false;
    overwriteAttackData->m_isReflectable = false;
    overwriteAttackData->m_isAbsorbable = false;
    overwriteAttackData->m_region = soCollisionAttackData::Region_None;

    overwriteAttackData->m_serialHitFrame = 30;
    overwriteAttackData->m_isDirect = false;
    overwriteAttackData->m_isInvalidInvincible = false;
    overwriteAttackData->m_isInvalidXlu = false;
    overwriteAttackData->m_lrCheck = soCollisionAttackData::Lr_Check_Pos;
    overwriteAttackData->m_isCatch = false;
    overwriteAttackData->m_noTeam = true;
    overwriteAttackData->m_noHitStop = false;
    overwriteAttackData->m_noEffect = false;
    overwriteAttackData->m_noTransaction = false;
    overwriteAttackData->m_subShield = 0;

    overwriteAttackData->m_shapeType = soCollision::Shape_Capsule;
}

void grBowserCastleThwomp::update(float deltaFrame){
    grMadein::update(deltaFrame);

    stBowserCastleData* stageData = static_cast<stBowserCastleData*>(this->getStageData());
    switch(this->state) {
        case State_Spawn:
            if (this->m_modelAnims[0]->getFrame() >= this->m_modelAnims[0]->getFrameCount()) {
                this->changeState(State_Sleep);
                Vec3f areaPosSW;
                Vec3f areaPosNE;
                this->getNodePosition(&areaPosSW, 0, "AreaSW");
                this->getNodePosition(&areaPosNE, 0, "AreaNE");
                Vec2f areaNW = Vec2f(areaPosSW.m_x, areaPosNE.m_y);
                Vec2f areaSE = Vec2f(areaPosNE.m_x, areaPosSW.m_y);
                this->dangerZoneId = g_aiMgr->setDangerZone(&areaNW, &areaSE, this->dangerZoneId, 0, 0);
            }
            break;
        case State_Awake:
            if (!this->isPlayerUnder) {
                this->changeState(State_Sleep);
            }
            break;
        case State_Shake:
            if (!this->isPlayerUnder) {
                this->changeState(State_Sleep);
                if (stageData->thwompShakeActivateFrames >= 0) {
                    this->timer = stageData->thwompShakeActivateFrames;
                }
            }
            break;
        case State_Stomp:
        {
            float currentFrame = this->m_modelAnims[0]->getFrame();
            if (currentFrame >= this->m_modelAnims[0]->getFrameCount() && this->dangerZoneId != -1) {
                g_aiMgr->delDangerZone(this->dangerZoneId);
                this->dangerZoneId = -1;
                this->stopGimmickEffect(1);
            }
            else if (currentFrame >= stageData->thwompEffectDropFrame && this->m_effects[1].m_handleId == -1) {
                this->startGimmickEffect(1);
            }
        }
        case State_Inactive:
            this->timer -= deltaFrame;
            if (this->timer <= 0) {
                this->changeState(State_Spawn);
            }
            break;
        default:
            break;
    }

    this->isPlayerUnder = false;
}

void grBowserCastleThwomp::onGimmickEvent(soGimmickEventArgs* eventInfo, int* taskId) {
    //if (!g_GameGlobal->m_modeMelee->m_meleeInitData.m_isHazardOff) {
        if (this->state != State_Stomp) {
            stBowserCastleData *stageData = static_cast<stBowserCastleData *>(this->getStageData());
            this->timer -= 1.0;
            this->isPlayerUnder = true;

            if (this->timer <= 0.0) {
                this->changeState(State_Stomp);
            } else if (this->timer <= fabsf(stageData->thwompShakeActivateFrames)) {
                this->changeState(State_Shake);
            } else {
                this->changeState(State_Awake);
            }
        }
    //}
}



void grBowserCastleThwomp::onInflictEach(soCollisionLog* collisionLog, float power) {
    this->m_soundEffects[2].m_generatorIndex = 2;
    this->startGimmickSE(2);
    Vec3f scale;
    this->getNodeScale(&scale, 0, "Quake");
    cmReqQuake(cmQuake::Amplitude_XL, &scale);
    if (this->shadow != NULL) {
        this->shadow->changeState(grBowserCastleShadow::State_Break);
    }
}

void grBowserCastleThwomp::changeState(State state) {

    if (this->state != state) {
        stBowserCastleData* stageData = static_cast<stBowserCastleData*>(this->getStageData());
        if (state == State_Awake && type == Type_Right) {
            this->setMotion(5);
        }
        else {
            this->setMotion(state);
        }
        switch(state) {
            case State_Spawn:
                this->setSleepArea(true);
                this->setSleepAttack(true);
                this->timer = randi(stageData->thwompMaxActivateFrames - stageData->thwompMinActivateFrames) + stageData->thwompMinActivateFrames;
                this->stopGimmickEffect(1);
                this->startGimmickSE(0);
                break;
            case State_Sleep:
                this->setSleepArea(false);
                this->setSleepAttack(true);
                this->stopGimmickEffect(0);
                this->stopGimmickSE(0);
                this->stopGimmickSE(1);
                break;
            case State_Shake:
                this->startGimmickEffect(0);
                this->m_soundEffects[1].m_generatorIndex = 1;
                this->startGimmickSE(1);
                break;
            case State_Stomp:
                this->setSleepArea(true);
                this->setSleepAttack(false);
                this->stopGimmickEffect(0);
                this->stopGimmickSE(1);
                this->m_soundEffects[3].m_generatorIndex = 3;
                this->startGimmickSE(3);
                this->timer = randi(stageData->thwompMaxRespawnFrames - stageData->thwompMinRespawnFrames) + stageData->thwompMinRespawnFrames;
                break;
            default:
                break;

        }
        this->state = state;
    }
    if (this->shadow != NULL) {
        this->shadow->changeState((grBowserCastleShadow::State)state);
    }
}

grBowserCastleThwomp::State grBowserCastleThwomp::getState() {
    return state;
}
