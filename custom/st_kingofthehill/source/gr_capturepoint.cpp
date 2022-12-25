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

    this->createSoundWork(1,1);
    this->m_soundEffects[0].m_id = snd_se_common_coin;
    this->m_soundEffects[0].m_0x10 = 0;
    this->m_soundEffects[0].m_nodeIndex = 0;
    this->m_soundEffects[0].m_0x14 = 0;
    this->m_soundEffects[0].m_0x1c = 0.0;
    this->m_soundEffects[0].m_0x20 = 0.0;

    Vec2f areaOffsetPos = {0.0,12.5};
    Vec2f areaRange = {25.0, 25.0}; // TODO: Expose values
    this->areaData = (soAreaData){ 0, 0x15, 0, 0, 0, 0, areaOffsetPos, areaRange };
    this->setAreaGimmick(&this->areaData, &this->areaInit, &this->areaInfo, false);
    stTrigger* trigger = g_stTriggerMng->createTrigger(GimmickKind_AreaCommon,-1);
    trigger->setObserveYakumono(this->m_yakumono);

    this->disableArea();
}

void grCapturePoint::update(float deltaFrame)
{
    grMadein::update(deltaFrame);
    switch(this->state) {
        case State_Off:
            break;
        case State_Appear:
            if (this->m_modelAnims[0]->m_anmObjMatClrRes->GetFrame() >= this->m_modelAnims[0]->m_anmObjMatClrRes->m_anmMatClrFile->m_animLength - 1) {
                this->state = State_On;
                this->setMotionDetails(0, 0, 0, 0, State_On);
                this->stage->zoomInCamera();
            }
            break;
        case State_Disappear:
            if (!this->isCaptured) {
                if (this->prevIsCaptured) {
                    this->consecutiveFramesCaptured = 0;
                }
            }
            else {
                this->consecutiveFramesCaptured += deltaFrame;
            }
            this->prevIsCaptured = this->isCaptured;
            this->isCaptured = false;
            if (this->m_modelAnims[0]->m_anmObjMatClrRes->GetFrame() >= this->m_modelAnims[0]->m_anmObjMatClrRes->m_anmMatClrFile->m_animLength - 1) {
                this->setNewCapturePosition();
            }
            break;
        case State_Out:
            if (this->m_modelAnims[0]->m_anmObjMatClrRes->GetFrame() >= this->m_modelAnims[0]->m_anmObjMatClrRes->m_anmMatClrFile->m_animLength - 1) {
                this->state = State_On;
                this->setMotionDetails(0, 0, 0, 0, State_On);
            }
        default:
            if (!this->isCaptured) {
                if (this->prevIsCaptured) {
                    this->state = State_Out;
                    this->setMotionDetails(0, 0, 0, 0, State_Out);
                }
                this->consecutiveFramesCaptured = 0;
            }
            else {
                this->consecutiveFramesCaptured += deltaFrame;
            }
            this->prevIsCaptured = this->isCaptured;
            this->isCaptured = false;
            break;
    }
}

void grCapturePoint::onGimmickEvent(soGimmickEventInfo* eventInfo, int* taskId)
{
    int entryId = g_ftManager->getEntryIdFromTaskId(*taskId, NULL);
    if (entryId >= 0) {
        if ((int)this->consecutiveFramesCaptured % 30 == 0) {
            if (this->rule == Rule_Coin) {
                g_ftManager->pickupCoin(entryId, 1);
                this->startGimmickSE(0);
            } else {
                g_ftManager->setHeal(entryId, 1.0); // TODO: Expose to STDT
            }
        }
        this->numCaptures++;
        if (this->state == State_Appear) {
            this->stage->zoomInCamera();
        }
        if (this->state != State_Disappear) {
            if (this->numCaptures >= 300) { // Expose to STDT
                this->state = State_Disappear;
                this->setMotionDetails(0, 0, 0, 0, State_Disappear);
            }
            if (!this->isCaptured && !this->prevIsCaptured) {
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
        this->isCaptured = true;
    }
}

void grCapturePoint::setCapturePointPositions(Ground* capturePointPositions) {
    this->capturePointPositions = capturePointPositions;
}

void grCapturePoint::setRule(Rule rule) {
    this->rule = rule;
}

void grCapturePoint::setNewCapturePosition() {
    u32 capturePointsIndex = this->capturePointPositions->getNodeIndex(0, "CapturePoints");
    u32 endIndex = this->capturePointPositions->getNodeIndex(0, "End");
    u32 nodeIndex;
    if (this->selectedNodeIndex < 0) {
        nodeIndex = randi(endIndex - capturePointsIndex - 1) + capturePointsIndex + 1;
    }
    else {
        nodeIndex = randi(endIndex - capturePointsIndex - 2) + capturePointsIndex + 1;
        if (nodeIndex >= this->selectedNodeIndex) {
            nodeIndex++;
        }
    }
    this->selectedNodeIndex = nodeIndex;
    nw4r::g3d::ResNodeData* resNodeData = this->capturePointPositions->m_sceneModels[0]->m_resMdl.GetResNode(nodeIndex).ptr();
    this->setPos(resNodeData->m_translation.m_x, resNodeData->m_translation.m_y, 0.0);
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

    grGimmickMotionPathInfo motionPathInfo = {this->stage->m_fileData, &this->motionPathData, 0x01000000, 0, 0, 0, 0, 0, 0 };
    stTrigger::TriggerData triggerData = (stTrigger::TriggerData){0,0,1,0};
    this->createAttachMotionPath(&motionPathInfo, &triggerData, "MoveNode");

    this->numCaptures = 0;
    this->isCaptured = false;
    this->prevIsCaptured = false;
    this->enableArea();
    this->state = State_Appear;
    this->setMotionDetails(0, 0, 0, 0, State_Appear);
    stRange range;
    this->stage->m_stagePositions->getCameraRange(&range);
    this->stage->zoomOutCamera((range.m_right - range.m_left)*2,(range.m_top - range.m_bottom)*2);

    // TODO: Play a sound effect
}

// TODO: Motion path



