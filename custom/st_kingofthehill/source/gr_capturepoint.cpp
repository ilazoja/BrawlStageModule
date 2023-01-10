#include "gr_capturepoint.h"
#include <ec/ec_mgr.h>
#include <memory.h>
#include <mt/mt_prng.h>
#include <OS/OSError.h>

grCapturePoint* grCapturePoint::create(int mdlIndex, char* tgtNodeName, char* taskName, stMelee* stage)
{
    grCapturePoint* ground = new (Heaps::StageInstance) grCapturePoint(taskName);
    ground->setMdlIndex(mdlIndex);
    ground->m_heapType = Heaps::StageInstance;
    ground->makeCalcuCallback(1, Heaps::StageInstance);
    ground->setCalcuCallbackRoot(7);
    ground->setupMelee();

    ground->stage = stage;

    return ground;
}

void grCapturePoint::startup(gfArchive* archive, u32 unk1, u32 unk2) {
    grMadein::startup(archive, unk1, unk2);

    stKingOfTheHillData* stageData = (stKingOfTheHillData*)this->getStageData();

    this->createSoundWork(3,1);
    this->m_soundEffects[0].m_id = snd_se_common_coin;
    this->m_soundEffects[0].m_0x10 = 0;
    this->m_soundEffects[0].m_nodeIndex = 0;
    this->m_soundEffects[0].m_0x14 = 0;
    this->m_soundEffects[0].m_0x1c = 0.0;
    this->m_soundEffects[0].m_0x20 = 0.0;
    this->m_soundEffects[1].m_id = snd_se_common_Countdown;
    this->m_soundEffects[1].m_0x10 = 0;
    this->m_soundEffects[1].m_nodeIndex = 0;
    this->m_soundEffects[1].m_0x14 = 0;
    this->m_soundEffects[1].m_0x1c = 0.0;
    this->m_soundEffects[1].m_0x20 = 0.0;
    this->m_soundEffects[2].m_id = snd_se_AllStar_Heal_Warp;
    this->m_soundEffects[2].m_0x10 = 0;
    this->m_soundEffects[2].m_nodeIndex = 0;
    this->m_soundEffects[2].m_0x14 = 0;
    this->m_soundEffects[2].m_0x1c = 0.0;
    this->m_soundEffects[2].m_0x20 = 0.0;

    this->areaData = (soAreaData){ 0, 0x15, 0, 0, 0, 0, stageData->areaOffsetPos, stageData->areaRange};
    this->setAreaGimmick(&this->areaData, &this->areaInit, &this->areaInfo, false);
    stTrigger* trigger = g_stTriggerMng->createTrigger(GimmickKind_AreaCommon,-1);
    trigger->setObserveYakumono(this->m_yakumono);

    this->disableArea();
    this->setPos(2*this->stage->m_deadRange.m_top, 0.0, 0.0);
}

void grCapturePoint::update(float deltaFrame)
{
    grMadein::update(deltaFrame);

    if (this->m_gimmickMotionPath == NULL && this->syncedGround != NULL) {
        Vec3f pos = this->syncedGround->getPos();
        this->setPos(&pos);
    }
    stKingOfTheHillData* stageData = (stKingOfTheHillData*)this->getStageData();

    switch(this->state) {
        case State_Off:
            break;
        case State_Appear:
            if (this->m_modelAnims[0]->m_anmObjMatClrRes->GetFrame() >= this->m_modelAnims[0]->m_anmObjMatClrRes->m_anmMatClrFile->m_animLength - 1) {
                this->state = State_On;
                this->setMotionDetails(0, 0, 0, 0, State_On);
                this->stage->zoomInCamera();
                if (this->collisionMode == CollisionMode_On) {
                    this->setEnableCollisionStatus(true);
                }
            }
            break;
        case State_Disappear:
            if (this->stayCapturedTimer > 0.0) {
                this->stayCapturedTimer -= deltaFrame;
                if (this->stayCapturedTimer <= 0.0) {
                    this->consecutiveFramesCaptured = 0;
                    this->bonusMultiplier = 1;
                    this->applyMotionRate(1.0);
                }
                else {
                    this->consecutiveFramesCaptured += deltaFrame;
                    if (stageData->consecutiveFramesBeforeBonus > 0 && int(this->consecutiveFramesCaptured) % stageData->consecutiveFramesBeforeBonus == 0 && this->bonusMultiplier <= 5) {
                        this->bonusMultiplier++;
                    }
                }
            }

            if (int(this->m_modelAnims[0]->m_anmObjMatClrRes->GetFrame()) % 45 == 0) {
                this->startGimmickSE(1);
            }
            if (this->m_modelAnims[0]->m_anmObjMatClrRes->GetFrame() >= this->m_modelAnims[0]->m_anmObjMatClrRes->m_anmMatClrFile->m_animLength - 1) {
                this->setNewCapturePosition();
            }
            break;
        case State_Out:
            if (this->m_modelAnims[0]->m_anmObjMatClrRes->GetFrame() >= this->m_modelAnims[0]->m_anmObjMatClrRes->m_anmMatClrFile->m_animLength - 1) {
                this->state = State_On;
                this->setMotionDetails(0, 0, 0, 0, State_On);
                if (this->collisionMode == CollisionMode_CaptureOnly) {
                    this->setEnableCollisionStatus(false);
                }
            }
        default:
            if (this->stayCapturedTimer > 0.0) {
                this->stayCapturedTimer -= deltaFrame;
                if (this->stayCapturedTimer <= 0.0) {
                    this->state = State_Out;
                    this->setMotionDetails(0, 0, 0, 0, State_Out);
                    this->consecutiveFramesCaptured = 0;
                    this->bonusMultiplier = 1;
                    this->applyMotionRate(1.0);
                }
                else {
                    this->consecutiveFramesCaptured += deltaFrame;
                    if (stageData->consecutiveFramesBeforeBonus > 0 && int(this->consecutiveFramesCaptured) % stageData->consecutiveFramesBeforeBonus == 0 && this->bonusMultiplier <= 5) {
                        this->bonusMultiplier++;
                    }
                }
            }
            break;
    }
}

void grCapturePoint::onGimmickEvent(soGimmickEventInfo* eventInfo, int* taskId)
{
    int entryId = g_ftManager->getEntryIdFromTaskId(*taskId, NULL);
    if (entryId >= 0) {
        stKingOfTheHillData* stageData = static_cast<stKingOfTheHillData*>(this->getStageData());

        Fighter* fighter = g_ftManager->getFighter(entryId, 0);
        if (!stageData->disableCapturesDuringShielding || fighter->m_moduleAccesser->getStatusModule()->getStatusKind() != ftStatus::Shield) {
            this->applyMotionRate(stageData->bonusMotionSpeedMultipliers[this->bonusMultiplier - 1]);
            if (this->consecutiveFramesCaptured >= stageData->consecutiveFramesBeforeStartReward && int(this->consecutiveFramesCaptured) % stageData->rewardRate == 0) {
                if (this->gameRule == Game_Rule_Coin) {
                    g_ftManager->pickupCoin(entryId, 1*this->bonusMultiplier);
                    this->startGimmickSE(0);
                } else {
                    g_ftManager->setHeal(entryId, stageData->healAmount*this->bonusMultiplier);
                }
            }
            this->numCaptures++;
            if (this->state == State_Appear) {
                this->stage->zoomInCamera();
                if (this->collisionMode == CollisionMode_On || this->collisionMode == CollisionMode_CaptureOnly) {
                    this->setEnableCollisionStatus(true);
                }
            }
            else if (this->state == State_On) {
                if (this->collisionMode == CollisionMode_CaptureOnly) {
                    this->setEnableCollisionStatus(true);
                }
            }

            if (this->state != State_Disappear) {
                if (this->targetNumCaptures > 0 && this->numCaptures >= this->targetNumCaptures) {
                    this->state = State_Disappear;
                    this->setMotionDetails(0, 0, 0, 0, State_Disappear);
                }
                if (this->stayCapturedTimer <= 0.0) {
                    this->state = State_In;
                    this->setMotionDetails(0, 0, 0, 0, State_In);
                }
                else if (this->state == State_In){
                    if (this->m_modelAnims[0]->m_anmObjMatClrRes->GetFrame() >= this->m_modelAnims[0]->m_anmObjMatClrRes->m_anmMatClrFile->m_animLength - 1) {
                        this->state = State_Capturing;
                        this->setMotionDetails(0, 0, 0, 0, State_Capturing);
                    }
                }
            }
            this->stayCapturedTimer = stageData->framesBeforeStopCapture;
        }
    }
}

void grCapturePoint::setCapturePointPositions(Ground* capturePointPositions) {
    this->capturePointPositions = capturePointPositions;
}

void grCapturePoint::setGameRule(GameRule gameRule) {
    this->gameRule = gameRule;
}

void grCapturePoint::setNewCapturePosition() {
    stKingOfTheHillData* stageData = static_cast<stKingOfTheHillData*>(this->getStageData());
    u32 capturePointsIndex = this->capturePointPositions->getNodeIndex(0, "CapturePoints");
    u32 endIndex = this->capturePointPositions->getNodeIndex(0, "End");
    u32 nodeIndex;
    if (this->selectedNodeIndex < 0) {
        nodeIndex = randi(endIndex - capturePointsIndex - 1) + capturePointsIndex + 1;
    }
    else {
        // Skip currently selected node
        nodeIndex = randi(endIndex - capturePointsIndex - 2) + capturePointsIndex + 1;
        if (nodeIndex >= this->selectedNodeIndex) {
            nodeIndex++;
        }
    }
    this->selectedNodeIndex = nodeIndex;
    nw4r::g3d::ResNodeData* resNodeData = this->capturePointPositions->m_sceneModels[0]->m_resMdl.GetResNode(nodeIndex).ptr();
    this->setPos(resNodeData->m_translation.m_x, resNodeData->m_translation.m_y, 0.0);
    this->setRot(0.0, 0.0, resNodeData->m_rotation.m_z);
    this->setScale(&resNodeData->m_scale);
    this->setEnableCollisionStatus(false);
    this->collisionMode = static_cast<CollisionMode>(int(resNodeData->m_rotation.m_y));
    this->startGimmickSE(2);

    this->motionPathData.m_motionRatio = 1.0;
    this->motionPathData.m_index = 0;
    this->motionPathData.m_0x5 = 1;
    this->motionPathData.m_mdlIndex = resNodeData->m_translation.m_z;
    this->motionPathData._padding = 0x0;
    if (this->m_gimmickMotionPath != NULL) {
        gfTask* task = this->m_attachedTask;
        if (task == this->m_gimmickMotionPath) {
            this->m_attachedTask = NULL;
        }
        else {
            gfTask* lastTask;
            while (task != this->m_gimmickMotionPath) {
                lastTask = task;
                task = task->m_nextTask;

            }
            if (task->m_nextTask != NULL) {
                lastTask->m_nextTask = task->m_nextTask;
            }
            else {
                lastTask->m_nextTask = NULL;
            }
        }
        this->m_gimmickMotionPath->preExit();
        this->m_gimmickMotionPath->exit();
        this->m_gimmickMotionPath = NULL;
    }

    this->applyMotionRate(1.0);
    if (this->syncedGround != NULL && this->syncedGroundExitAnim >= 0) {
        this->syncedGround->setMotion(this->syncedGroundExitAnim);
    }
    this->syncedGround = NULL;
    this->syncedGroundExitAnim = -1;
    grGimmickMotionPathInfo motionPathInfo = {this->stage->m_fileData, &this->motionPathData, 0x01000000, 0, 0, 0, 0, 0, 0 };
    stTrigger::TriggerData triggerData = (stTrigger::TriggerData){0,0,1,0};
    this->createAttachMotionPath(&motionPathInfo, &triggerData, "MoveNode");
    if (resNodeData->m_rotation.m_x > 0) {
        this->syncedGround = static_cast<grMadein*>(this->stage->getGround(resNodeData->m_rotation.m_x));
        if (this->syncedGround != NULL) {
            if (resNodeData->m_translation.m_x >= 0) {
                this->syncedGround->setMotion(resNodeData->m_translation.m_x);
                this->syncedGroundExitAnim = resNodeData->m_translation.m_y;
            }
            if (this->m_gimmickMotionPath != NULL) {
                if (this->syncedGround->m_gimmickMotionPath != NULL) {
                    this->m_gimmickMotionPath->setFrame(this->syncedGround->m_gimmickMotionPath->getFrame());
                }
                else {
                    this->m_gimmickMotionPath->setFrame(this->syncedGround->getMotionFrame(0));
                }
            }
        }
    }

    this->targetNumCaptures = randf()*(stageData->maxCapturesBeforeRelocate - stageData->minCapturesBeforeRelocate) + stageData->minCapturesBeforeRelocate;
    this->numCaptures = 0;
    this->stayCapturedTimer = 0.0;
    this->consecutiveFramesCaptured = 0;
    this->bonusMultiplier = 1;
    this->enableArea();
    this->state = State_Appear;
    this->setMotionDetails(0, 0, 0, 0, State_Appear);
    stRange range;

    if (!stageData->disableCameraZoom) {
        this->stage->m_stagePositions->getCameraRange(&range);
    }
    this->stage->zoomOutCamera((range.m_right - range.m_left)*2,(range.m_top - range.m_bottom)*2);
}

void grCapturePoint::applyMotionRate(float motionRate) {
    if (this->m_gimmickMotionPath != NULL) {
        this->m_gimmickMotionPath->setFrameUpdate(motionRate);
    }
    if (this->syncedGround != NULL) {
        this->syncedGround->setMotionRatio(motionRate);
    }
}