#include "gr_ghosthouse_hole.h"
#include "st_ghosthouse_data.h"
#include <ec/ec_mgr.h>
#include <memory.h>
#include <stdio.h>
#include <mt/mt_prng.h>

grGhostHouseHole* grGhostHouseHole::create(int mdlIndex, const char* tgtNodeName, const char* taskName)
{
    grGhostHouseHole* ground = new (Heaps::StageInstance) grGhostHouseHole(taskName);
    ground->setMdlIndex(mdlIndex);
    ground->m_heapType = Heaps::StageInstance;
    ground->makeCalcuCallback(1, Heaps::StageInstance);
    ground->setCalcuCallbackRoot(7);
    ground->setupMelee();

    return ground;
}

void grGhostHouseHole::setMotionPathData(int mdlIndex) {
    this->motionPathData.set(1.0, 0, grGimmickMotionPathData::Path_Loop, mdlIndex, 0);
}

void grGhostHouseHole::startup(gfArchive* archive, u32 unk1, u32 unk2) {
    grMadein::startup(archive, unk1, unk2);

    grGimmickMotionPathInfo motionPathInfo(archive, &this->motionPathData, false, true);
    stTriggerData triggerData(0,true,stTriggerData::Keep_None);
    this->createAttachMotionPath(&motionPathInfo, &triggerData, "MoveNode");

    this->changeState(State_Inactive);
}

void grGhostHouseHole::update(float deltaFrame)
{
    grMadein::update(deltaFrame);
    this->updateStatus(deltaFrame);
    this->updateColl(deltaFrame);
}

void grGhostHouseHole::updateColl(float deltaFrame) {
    Vec3f posSW;
    this->getNodePosition(&posSW, 0, "NodeSW");
    Vec3f posNE;
    this->getNodePosition(&posNE, 0, "NodeNE");

    for (u32 i = 0; i < this->m_collision->m_lineLen; i++) {
        grCollisionLine* collisionLine = this->m_collision->getLine(i);
        clSegment2D segment;
        this->m_collision->getSegment(&segment, collisionLine);
        if (segment.m_p0Pos.m_x >= posSW.m_x && segment.m_p0Pos.m_x < posNE.m_x
            && segment.m_p1Pos.m_x >= posSW.m_x && segment.m_p1Pos.m_x < posNE.m_x
            && segment.m_p0Pos.m_y >= posSW.m_y && segment.m_p0Pos.m_y < posNE.m_y
            && segment.m_p1Pos.m_y >= posSW.m_y && segment.m_p1Pos.m_y < posNE.m_y) {
            collisionLine->m_isTargetAll = false;
            collisionLine->m_isUnder = false;

        }
        else {
            collisionLine->m_isTargetAll = true;
            collisionLine->m_isUnder = true;
        }
    }
}

void grGhostHouseHole::updateStatus(float deltaFrame) {
    switch (this->state) {
        case State_Appear:
            if (this->m_modelAnims[0]->getFrame() >= this->m_modelAnims[0]->getFrameCount()) {
                this->changeState(State_Active);
            }
            break;
        case State_Active:
            this->timer -= deltaFrame;
            if (this->timer <= 0) {
                this->changeState(State_Disappear);
            }
            break;
        case State_Disappear:
            if (this->m_modelAnims[0]->getFrame() >= this->m_modelAnims[0]->getFrameCount()) {
                this->changeState(State_Inactive);
            }
            break;
        case State_Inactive:
            this->timer -= deltaFrame;
            if (this->timer <= 0) {
                this->changeState(State_Appear);
            }
            break;
        default:
            break;
    }

}

void grGhostHouseHole::changeState(State state) {
    stGhostHouseData *ghostHouseData = static_cast<stGhostHouseData *>(this->getStageData());
    if (this->state != state) {
        switch (state) {
            case State_Appear:
                this->m_gimmickMotionPath->setFrameUpdate(0.0);
                break;
            case State_Active:
                this->timer = randi(ghostHouseData->holeActiveMaxFrames - ghostHouseData->holeActiveMinFrames) + ghostHouseData->holeActiveMinFrames;
                this->m_gimmickMotionPath->setFrameUpdate(1.0);
                break;
            case State_Disappear:
                this->m_gimmickMotionPath->setFrameUpdate(0.0);
                break;
            case State_Inactive:
                this->timer = randi(ghostHouseData->holeSpawnMaxFrames - ghostHouseData->holeSpawnMinFrames) + ghostHouseData->holeSpawnMinFrames;
                this->m_gimmickMotionPath->setFrameUpdate(1.0);
                break;
            default:
                break;
        }
        this->setMotion(state);
        this->state = state;
    }
}



