#include "st_izumi_data.h"
#include "gr_izumi_spout.h"
#include <ec/ec_mgr.h>
#include <memory.h>
#include <mt/mt_vector.h>
#include <mt/mt_prng.h>
#include <ft/ft_manager.h>
#include <it/it_manager.h>
#include <st/stage.h>
#include <gm/gm_global.h>
#include <math.h>
#include <hkmath/hkMath.h>
#include <so/so_external_value_accesser.h>
#include <OS/OSError.h>

grIzumiSpout* grIzumiSpout::create(int mdlIndex, const char* tgtNodeName, const char* taskName)
{
    grIzumiSpout* ground = new (Heaps::StageInstance) grIzumiSpout(taskName);
    ground->setupMelee();
    ground->setMdlIndex(mdlIndex);
    ground->setTgtNode(tgtNodeName);
    return ground;
}

void grIzumiSpout::processAnim() {
    if (m_resFile.ptr() != NULL) {
        float updateRate = g_stGameFrame.m_frameDelta * this->m_motionRatio *
                g_GameGlobal->m_stageData->m_motionRatio * g_GameGlobal->m_stageData->m_motionSubRatio;
        if (this->m_isPauseAnim) {
            updateRate = 0.0;
        }
        for (int i = 0; i < this->m_modelAnimNum; i++) {
            if (this->anmChrIndex != MOVE_ANIM_CHR_INDEX) {
                if (this->m_modelAnims[i]->m_anmObjChrRes != NULL) {
                    this->m_modelAnims[i]->m_anmObjChrRes->SetUpdateRate(updateRate);
                }
            }
            if (this->anmVisIndex != MOVE_ANIM_CHR_INDEX) {
                if (this->m_modelAnims[i]->m_anmObjVisRes != NULL) {
                    this->m_modelAnims[i]->m_anmObjVisRes->SetUpdateRate(updateRate);
                }
            }
            if (this->m_modelAnims[i]->m_anmObjTexPatRes != NULL) {
                this->m_modelAnims[i]->m_anmObjTexPatRes->SetUpdateRate(updateRate);
            }
            if (this->anmSrtIndex != MOVE_ANIM_CHR_INDEX) {
                if (this->m_modelAnims[i]->m_anmObjTexSrtRes != NULL) {
                    this->m_modelAnims[i]->m_anmObjTexSrtRes->SetUpdateRate(updateRate);
                }
            }
            if (this->m_modelAnims[i]->m_anmObjMatClrRes != NULL) {
                this->m_modelAnims[i]->m_anmObjMatClrRes->SetUpdateRate(updateRate);
            }
            if (this->m_modelAnims[i]->m_anmObjShpRes != NULL) {
                this->m_modelAnims[i]->m_anmObjShpRes->SetUpdateRate(updateRate);
            }
            if (this->m_noUpdateAnim) {
                this->m_sceneModels[i]->G3dProc(8, 0, 0);
            }
            else {
                this->m_sceneModels[i]->UpdateFrame();
            }
        }

        this->updateG3dProcCalcWorld();
        this->m_hasUpdatedG3dCalcWorld = false;
    }
}


void grIzumiSpout::update(float deltaFrame)
{
    this->updateLevel(deltaFrame);
    this->updateColl(deltaFrame);
    this->updateEff(deltaFrame);
}

void grIzumiSpout::setMotion(u32 chrIndex, u32 srtIndex, u32 visIndex, u32 clrIndex) {
    float frame = this->m_modelAnims[0]->m_anmObjChrRes->GetFrame();
    int prevAnmChrIndex = this->anmChrIndex;

    if (chrIndex >= 0 && this->anmChrIndex != chrIndex) {
        this->changeNodeAnim(chrIndex, 0);
        if (chrIndex == MOVE_ANIM_CHR_INDEX) {
            this->m_modelAnims[0]->m_anmObjChrRes->SetUpdateRate(0.0);
        }
        this->anmChrIndex = chrIndex;
    }
    if (srtIndex >= 0 && this->anmSrtIndex != srtIndex) {
        this->changeTexSrtAnim(srtIndex, 0);
        if (srtIndex == MOVE_ANIM_CHR_INDEX) {
            this->m_modelAnims[0]->m_anmObjTexSrtRes->SetUpdateRate(0.0);
        }
        this->anmSrtIndex = srtIndex;
    }
    if (visIndex >= 0 && this->anmVisIndex != visIndex) {
        this->changeVisibleAnim(visIndex, 0);
        if (visIndex == MOVE_ANIM_CHR_INDEX) {
            this->m_modelAnims[0]->m_anmObjVisRes->SetUpdateRate(0.0);
        }
        this->anmVisIndex = visIndex;
    }
    if (clrIndex >= 0 && this->anmClrIndex != clrIndex) {
        this->changeMatColAnim(clrIndex, 0);
        this->anmClrIndex = clrIndex;
    }

    if (prevAnmChrIndex == MOVE_ANIM_CHR_INDEX && chrIndex == MOVE_ANIM_CHR_INDEX) {
        this->m_modelAnims[0]->m_anmObjChrRes->SetFrame(frame);
        this->m_modelAnims[0]->m_anmObjVisRes->SetFrame(frame);
        if (this->anmSrtIndex == MOVE_ANIM_CHR_INDEX) {
            this->m_modelAnims[0]->m_anmObjTexSrtRes->SetFrame(frame);
        }
    }
}

void grIzumiSpout::setFrame(float frame) {
    this->m_modelAnims[0]->m_anmObjChrRes->SetFrame(frame);
    this->m_modelAnims[0]->m_anmObjVisRes->SetFrame(frame);
    if (this->anmSrtIndex == MOVE_ANIM_CHR_INDEX) {
        this->m_modelAnims[0]->m_anmObjTexSrtRes->SetFrame(frame);
    }
}
void grIzumiSpout::fountainInit(u32 spoutId)
{
    stIzumiData* izumiData = static_cast<stIzumiData*>(this->getStageData());

    this->spoutId = spoutId;

	this->isActive = true;
    this->startFountainEffect();
    this->sndPtr = this->soundGenerator.playSE(snd_se_stage_Izumi_Spout, 0, 0, -1);

    if (g_GameGlobal->m_modeMelee->m_meleeInitData.m_isHazardOff) {
        this->level = Level_Off;
        this->setMotion(0, 0, 0, 0);
    }
    else {
        this->setMotion(MOVE_ANIM_CHR_INDEX, 3, 2, 0);
        this->spoutTimer = randf()*(izumiData->stationaryMaxFrames - izumiData->stationaryMinFrames) + izumiData->stationaryMinFrames;
        this->destFrame = izumiData->spoutStartFrames[spoutId];
        this->setFrame(this->destFrame);
        this->level = Level_Active;
    }

    this->areaData = (soAreaData){ 0, gfArea::Stage_Group_Gimmick_Normal, AREA_SHAPE_FLAG_FOLLOW_NODE, 0, 0, this->getNodeIndex(0, "Trigger"), Vec2f(0.0, 0.0), izumiData->areaRange};
    this->setAreaGimmick(&this->areaData, &this->areaInit, &this->areaInfo, false);
    stTrigger* trigger = g_stTriggerMng->createTrigger(Gimmick::Area_Common,-1);
    trigger->setObserveYakumono(this->m_yakumono);

    this->disableArea();
}

void grIzumiSpout::startFountainEffect()
{
    this->effFrameCount = 0.0;
    g_ecMgr->setDrawPrio(1);
    this->effPtr = g_ecMgr->setEffect((EfID)(0x330001+this->spoutId));
    g_ecMgr->setDrawPrio(0xffffffff);
    g_ecMgr->setParent(this->effPtr, this->m_sceneModels[0], "Splash", 0);
}
void grIzumiSpout::stopFountainEffect()
{
    g_ecMgr->endEffect(this->effPtr);
}

void grIzumiSpout::updateEff(float deltaFrame)
{
    stIzumiData* izumiData = static_cast<stIzumiData*>(this->getStageData());

    Vec3f bonePos;
    this->getNodePosition(&bonePos, 0, "Splash");
    this->soundGenerator.setPos(&bonePos);
    if (bonePos.m_y < izumiData->spoutEffectMinHeight - 0.5 && this->isActive)
    {
        this->stopFountainEffect();
        this->soundGenerator.stopSE(this->sndPtr, 0);
        this->isActive = false;
    }
    else if (bonePos.m_y > izumiData->spoutEffectMinHeight && !this->isActive)
    {
        this->startFountainEffect();
        this->sndPtr = this->soundGenerator.playSE(snd_se_stage_Izumi_Spout, 0, 0, -1);
        this->isActive = true;
    }
    else if (this->isActive) {
        this->effFrameCount += deltaFrame;
        if (this->effFrameCount > izumiData->spoutEffectDuration) {
            this->stopFountainEffect();
            this->startFountainEffect();
            this->effFrameCount = 0;
        }
    }
}

void grIzumiSpout::updateColl(float deltaFrame) {
    stIzumiData* izumiData = static_cast<stIzumiData*>(this->getStageData());

    Vec3f bonePos;
    this->getNodePosition(&bonePos, 0, "Platform");
    if (bonePos.m_y >= izumiData->spoutCollisionMinHeight) {
        this->setEnableCollisionStatus(true);
    }
    else {
        this->setEnableCollisionStatus(false);
    }
}

void grIzumiSpout::updateLevel(float deltaFrame) {
    stIzumiData* izumiData = static_cast<stIzumiData*>(this->getStageData());

    if (this->level != Level_Off) {
        float currentAnimFrame = this->m_modelAnims[0]->getFrame();
        float animFrameCount = this->m_modelAnims[0]->getFrameCount();

        if (this->spoutTimer > 0) {
            if (this->anmChrIndex != MOVE_ANIM_CHR_INDEX || this->m_modelAnims[0]->m_anmObjChrRes->GetFrame() == this->destFrame) {
                this->spoutTimer -= deltaFrame;
            }
        }
        if (this->spoutTimer <= 0 && (currentAnimFrame >= animFrameCount - deltaFrame || this->anmChrIndex != WAIT_SINK_RIPPLE_ANIM_INDEX)) {
            this->changeDestFrame();
        }
        else if (this->spoutTimer <= izumiData->riseWarningFrames && this->level == Level_Sink) {
            this->setMotion(WAIT_SINK_RIPPLE_ANIM_INDEX, WAIT_SINK_RIPPLE_ANIM_INDEX, WAIT_SINK_RIPPLE_ANIM_INDEX, WAIT_SINK_RIPPLE_ANIM_INDEX);
        }
        else {
            float frame = this->m_modelAnims[0]->m_anmObjChrRes->GetFrame();

            float dir = this->destFrame - frame;
            if (dir > 0) {
                frame += deltaFrame*izumiData->spoutAscentSpeed;
                if (frame >= this->destFrame) {
                    frame = this->destFrame;
                }
            }
            else if (dir < 0) {
                frame -= this->isForceDown ? deltaFrame*izumiData->spoutDescentSpeed*SPOUT_FORCE_DOWN_MUL : deltaFrame*izumiData->spoutDescentSpeed;
                if (frame <= this->destFrame) {
                    frame = this->destFrame;
                    this->isForceDown = false;
                }
            }

            this->setFrame(frame);

            if (frame >= izumiData->spoutPassiveFrame) {
                this->setMotion(2, 3, 2, 0);
            }
            else {
                this->setMotion(2, 2, 2, 0);
            }
        }
    }
}

void grIzumiSpout::changeDestFrame() {
    stIzumiData* izumiData = static_cast<stIzumiData*>(this->getStageData());

    if (this->level == Level_Active) {
        if (randf() >= izumiData->spoutSinkChance) {
            float deltaFrame = randf() * (izumiData->spoutMaxMove - izumiData->spoutMinMove) + izumiData->spoutMinMove;
            if (this->destFrame >= izumiData->spoutMaxFrame) {
                deltaFrame *= -1;
            }
            else if (this->destFrame > izumiData->spoutMinFrame) {
                deltaFrame *= -1.0f + 2 * randi(2);
            }

            this->spoutTimer = randf() * (izumiData->stationaryMaxFrames - izumiData->stationaryMinFrames) +
                                   izumiData->stationaryMinFrames;
            this->destFrame += deltaFrame;
            this->destFrame = hkMath::min2<float>(this->destFrame, izumiData->spoutMaxFrame);
            this->destFrame = hkMath::max2<float>(this->destFrame, izumiData->spoutMinFrame);
        }
        else {
            this->level = Level_Sink;
            this->spoutTimer =
                    randf() * (izumiData->sinkMaxFrames - izumiData->sinkMinFrames) + izumiData->sinkMinFrames;
            this->destFrame = 0;
            this->enableArea();
        }
    }
    else {
        this->isForceDown = false;
        this->isItemObtained = false;
        this->spoutTimer = randf() * (izumiData->stationaryMaxFrames - izumiData->stationaryMinFrames) +
                           izumiData->stationaryMinFrames;
        this->destFrame = izumiData->spoutReturnFrame;
        this->level = Level_Active;
        this->setMotion(2, 2, 2, 0);
        this->disableArea();
    }
}

void grIzumiSpout::onGimmickEvent(soGimmickEventInfo* eventInfo, int* taskId)
{
    int entryId = g_ftManager->getEntryIdFromTaskId(*taskId, NULL);
    if (entryId >= 0) {
        stIzumiData* izumiData = static_cast<stIzumiData*>(this->getStageData());

        if (this->level == Level_Sink) {
            if (this->spoutTimer <= izumiData->riseWarningFrames) {
                Fighter *fighter = g_ftManager->getFighter(entryId, -1);
                if (fighter != NULL) {
                    if (!this->isItemObtained &&
                        soExternalValueAccesser::getStatusKind(fighter) == Fighter::Status_Warpstar_Jump) {
                        if (g_GameGlobal->m_modeMelee->m_meleeInitData.m_itSwitch.m_item.ex.m_stage) {
                            Vec3f fighterPos = soExternalValueAccesser::getPos(fighter);
                            Vec3f triggerPos;
                            this->getNodePosition(&triggerPos, 0, "Trigger");
                            float x = fabsf(fighterPos.m_x - triggerPos.m_x);
                            float radius = izumiData->areaRange.m_x / 2;
                            int i = 0;
                            for (i = 0; i < SPOUT_FOOD_AMOUNT; i++) {
                                if (x < (i+1)*radius/SPOUT_FOOD_AMOUNT){
                                    break;
                                }
                            }

                            itManager *itemManager = itManager::getInstance();
                            itKind kind = i == 0 ? Item_MaximTomato : Item_Food;
                            int amount = kind == Item_MaximTomato ? 1 : SPOUT_FOOD_AMOUNT + 1 - i;
                            for (u32 i = 0; i < amount; i++) {
                                BaseItem *item = itemManager->createItem(kind, 5000);
                                if (item != NULL) {
                                    item->warp(&triggerPos);
                                    Vec3f speed = Vec3f(0.0, SPOUT_ITEM_APPEAR_SPEED_Y, 0.0);
                                    item->m_moduleAccesser->getKineticModule().addSpeed(&speed,
                                                                                         item->m_moduleAccesser);
                                }
                            }
                        }
                        this->isItemObtained = true;
                    } else if (fighter->m_moduleAccesser->getItemManageModule().getHaveItemKind(0) == Item_StarRod &&
                               soExternalValueAccesser::getStatusKind(fighter) == Fighter::Status_Item_Swing_S4_Hold) {
                        soItemInfo itemInfo;
                        fighter->m_moduleAccesser->getItemManageModule().getHaveItemInfo(&itemInfo, 0);
                        if (itemInfo.m_item != NULL &&
                            soExternalValueAccesser::getWorkInt(itemInfo.m_item, BaseItem::Instance_Work_Int_Value_1) > 0) {
                            ipPadButton button = fighter->m_moduleAccesser->getControllerModule().getButton();
                            if (button.m_appealLw) {
                                this->spoutTimer =
                                        randf() * (izumiData->sinkMaxFrames - izumiData->sinkMinFrames) +
                                        izumiData->sinkMinFrames;
                                this->setMotion(2, 2, 2, 0);
                                this->setFrame(0);
                            }
                        }
                    }
                }
            }
            else {
                Fighter *fighter = g_ftManager->getFighter(entryId, -1);
                if (fighter != NULL) {
                    if (fighter->m_moduleAccesser->getItemManageModule().getHaveItemKind(0) == Item_StarRod &&
                        soExternalValueAccesser::getStatusKind(fighter) == Fighter::Status_Item_Swing_S4_Hold) {
                        soItemInfo itemInfo;
                        fighter->m_moduleAccesser->getItemManageModule().getHaveItemInfo(&itemInfo, 0);
                        if (itemInfo.m_item != NULL &&
                            soExternalValueAccesser::getWorkInt(itemInfo.m_item, BaseItem::Instance_Work_Int_Value_1) > 0) {
                            ipPadButton button = fighter->m_moduleAccesser->getControllerModule().getButton();
                            if (button.m_appealHi) {
                                this->spoutTimer = izumiData->riseWarningFrames;
                            }
                        }
                    }
                }
            }
            if (!this->isItemObtained && this->spoutTimer < izumiData->cooldownRisingFrames) {
                this->spoutTimer = izumiData->cooldownRisingFrames;
            }
        }
    }
}

void grIzumiSpout::receiveCollMsg_Landing(grCollStatus* collStatus, grCollisionJoint* collisionJoint, bool unk3) {
    if (this->level != Level_Off) {
        stIzumiData* izumiData = static_cast<stIzumiData*>(this->getStageData());
        CategoryFlag categoryFlagFighter(GROUND_COLL_STATUS_OWNER_TASK_CATEGORY_MASK_FIGHTER);
        if (this->isCollisionStatusOwnerTask(collStatus, &categoryFlagFighter)) {
            int entryId = g_ftManager->getEntryIdFromTaskId(collStatus->m_taskId, NULL);
            if (entryId >= 0) {
                Fighter *fighter = g_ftManager->getFighter(entryId, -1);
                if (fighter != NULL) {
                    if (fighter->m_moduleAccesser->getItemManageModule().getHaveItemKind(0) == Item_StarRod &&
                        soExternalValueAccesser::getStatusKind(fighter) == Fighter::Status_Item_Swing_S4_Hold) {
                        soItemInfo itemInfo;
                        fighter->m_moduleAccesser->getItemManageModule().getHaveItemInfo(&itemInfo, 0);
                        if (itemInfo.m_item != NULL &&
                            soExternalValueAccesser::getWorkInt(itemInfo.m_item, BaseItem::Instance_Work_Int_Value_1) > 0) {
                            ipPadButton button = fighter->m_moduleAccesser->getControllerModule().getButton();
                            if (button.m_appealHi) {
                                this->destFrame += izumiData->spoutAscentSpeed;
                                this->destFrame = hkMath::min2<float>(this->destFrame,
                                                                      izumiData->spoutMaxFrame);
                                this->spoutTimer =
                                        randf() *
                                        (izumiData->stationaryMaxFrames - izumiData->stationaryMinFrames) +
                                        izumiData->stationaryMinFrames;
                                this->level = Level_Active;
                            } else if (button.m_appealLw && this->destFrame >= izumiData->spoutMinFrame) {
                                this->destFrame -= izumiData->spoutAscentSpeed;
                                this->destFrame = hkMath::max2<float>(this->destFrame,
                                                                      izumiData->spoutMinFrame);
                                this->spoutTimer =
                                        randf() *
                                        (izumiData->stationaryMaxFrames - izumiData->stationaryMinFrames) +
                                        izumiData->stationaryMinFrames;
                            }
                        }
                    }
                }
            }
        }

        CategoryFlag categoryFlagItem(GROUND_COLL_STATUS_OWNER_TASK_CATEGORY_MASK_ITEM);
        if (this->isCollisionStatusOwnerTask(collStatus, &categoryFlagItem)) {
            BaseItem *item = static_cast<BaseItem *>(gfTask::getTask(collStatus->m_taskId));
            if (item != NULL && item->m_kind == Item_WarpStar &&
                soExternalValueAccesser::getStatusKind(item) == BaseItem::Status_Have) {
                float currentFrame = this->m_modelAnims[0]->m_anmObjChrRes->GetFrame();
                float deltaFrame = (izumiData->spoutMaxMove + izumiData->spoutMinMove) / 2;
                if (currentFrame - deltaFrame < izumiData->spoutMinFrame) {
                    this->level = Level_Sink;
                    this->spoutTimer =
                            randf() * (izumiData->sinkMaxFrames - izumiData->sinkMinFrames) + izumiData->sinkMinFrames;
                    this->destFrame = 0;
                    this->enableArea();
                }
                else {
                    this->destFrame = currentFrame - deltaFrame;
                    this->spoutTimer = randf() * (izumiData->stationaryMaxFrames - izumiData->stationaryMinFrames) +
                                       izumiData->stationaryMinFrames;
                }
                this->isForceDown = true;
            }
        }
    }
}
