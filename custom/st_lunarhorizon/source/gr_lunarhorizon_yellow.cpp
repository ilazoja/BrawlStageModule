#include "gr_lunarhorizon_yellow.h"
#include <ec/ec_mgr.h>
#include <memory.h>

grLunarHorizonYellow* grLunarHorizonYellow::create(int mdlIndex, const char* tgtNodeName, const char* taskName)
{
    grLunarHorizonYellow* ground = new (Heaps::StageInstance) grLunarHorizonYellow(taskName);
    ground->setupMelee();
    ground->setMdlIndex(mdlIndex);
    ground->m_heapType = Heaps::StageInstance;
    ground->makeCalcuCallback(1, Heaps::StageInstance);
    ground->setCalcuCallbackRoot(7);
    return ground;
}

void grLunarHorizonYellow::startup(gfArchive* data, u32 unk1, u32 unk2)
{
    grMadein::startup(data, unk1, unk2);
    this->createSoundWork(1,1);
    this->m_soundEffects[0].m_id = snd_se_stage_Pictchat_missile_exp;
    this->m_soundEffects[0].m_repeatFrame = 0;
    this->m_soundEffects[0].m_nodeIndex = 0;
    this->m_soundEffects[0].m_endFrame = 0;
    this->m_soundEffects[0].m_offsetPos = Vec2f(0.0, 0.0);
}
void grLunarHorizonYellow::update(float deltaFrame)
{
    stLunarHorizonData* stageData = static_cast<stLunarHorizonData*>(this->getStageData());
    if (this->turnOffTimer > 0) {
        this->turnOffTimer -= deltaFrame;
    }
    else {
        if (*this->cooldownTimerWork <= 0 && !this->isActive && (g_soWorld->m_gravityUp >= stageData->maxGravityUpBeforeResetAvailable || g_soWorld->m_gravityUp <= stageData->minGravityUpBeforeResetAvailable)) {
            this->isActive = true;
            this->setMotion(1);
        }
    }
    if (this->isActive && (*this->cooldownTimerWork > 0 || (g_soWorld->m_gravityUp < stageData->maxGravityUpBeforeResetAvailable && g_soWorld->m_gravityUp > stageData->minGravityUpBeforeResetAvailable))) {
        this->isActive = false;
        this->setMotion(0);
    }


}

void grLunarHorizonYellow::receiveCollMsg_Landing(grCollStatus* collStatus, grCollisionJoint* collisionJoint, bool unk3) {
    stLunarHorizonData* stageData = static_cast<stLunarHorizonData*>(this->getStageData());

    if (this->isActive) {
        this->isActive = false;
        this->setMotion(0);
        this->startGimmickSE(0);
        g_soWorld->m_gravityUp = stageData->startingGravityUp;
        g_soWorld->m_gravityDown = stageData->startingGravityDown;
        *this->cooldownTimerWork = stageData->cooldownFrames;
        *this->isResetWork = true;
    }

    this->turnOffTimer = 10.0;
}

void grLunarHorizonYellow::setCooldownWork(float* cooldownTimerWork) {
    this->cooldownTimerWork = cooldownTimerWork;
}

void grLunarHorizonYellow::setIsResetWork(bool* isResetWork) {
    this->isResetWork = isResetWork;
}

