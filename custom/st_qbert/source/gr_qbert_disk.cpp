#include <memory.h>
#include "gr_qbert_disk.h"
#include <mt/mt_prng.h>
#include <OS/OSError.h>

grQbertDisk* grQbertDisk::create(int mdlIndex, const char* tgtNodeName, const char* taskName){
    grQbertDisk* ground = new(Heaps::StageInstance) grQbertDisk(taskName);
    ground->setMdlIndex(mdlIndex);
    ground->m_heapType = Heaps::StageInstance;
    ground->makeCalcuCallback(1, Heaps::StageInstance);
    ground->setCalcuCallbackRoot(7);
    return ground;
}

void grQbertDisk::update(float frameDelta){
    if (*this->isActiveWork) {
        if (!this->prevIsActive) {
            this->setEnableCollisionStatus(true);
            this->setMotionDetails(0, 1, 0, 0, 0);
        }
        else if (this->m_modelAnims[0]->getFrame() - this->m_modelAnims[0]->getFrameCount() >= 0) {
            this->setEnableCollisionStatus(false);
            this->setMotionDetails(0, 0, 0, 0, 0);
            *this->isActiveWork = false;
            this->isMoving = false;
        }
    }

    this->prevIsActive = *this->isActiveWork;
}

void grQbertDisk::receiveCollMsg_Landing(grCollStatus* collStatus, grCollisionJoint* collisionJoint, bool unk3) {
    if (!isMoving) {
        CollCategoryFlag categoryFlag(COLL_CATEGORY_MASK_FIGHTER);
        if (this->isCollisionStatusOwnerTask(collStatus, &categoryFlag)) {
            this->setMotionDetails(1, 1, 0, 0, 0);
            this->soundGenerator.playSE(snd_se_stage_Madein_good_05, 0x0, 0x0, 0xffffffff);
            this->isMoving = true;
        }
    }
}

void grQbertDisk::setIsActiveWork(bool* isActiveWork) {
    this->isActiveWork = isActiveWork;
}

