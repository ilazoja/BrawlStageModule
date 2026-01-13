#include "gr_templeoftime_door.h"
#include <ec/ec_mgr.h>
#include <memory.h>
#include <ft/ft_manager.h>
#include <OS/OSError.h>
#include <sc/sc_melee.h>

grTempleOfTimeDoor* grTempleOfTimeDoor::create(int mdlIndex, const char* tgtNodeName, const char* taskName)
{
    grTempleOfTimeDoor* ground = new (Heaps::StageInstance) grTempleOfTimeDoor(taskName);
    ground->setupMelee();
    ground->setMdlIndex(mdlIndex);
    ground->m_heapType = Heaps::StageInstance;
    ground->makeCalcuCallback(1, Heaps::StageInstance);
    ground->setCalcuCallbackRoot(7);
    return ground;
}

void grTempleOfTimeDoor::startup(gfArchive* archive, u32 unk1, gfSceneRoot::LayerType layerType) {
    grMadein::startup(archive, unk1, layerType);
    this->createSoundWork(6,1);
    this->m_soundEffects[0].m_id = snd_se_stage_Madein_01;
    this->m_soundEffects[0].m_repeatFrame = 0;
    this->m_soundEffects[0].m_nodeIndex = 0;
    this->m_soundEffects[0].m_endFrame = 0;
    this->m_soundEffects[0].m_offsetPos = Vec2f(0.0, 0.0);
    this->m_soundEffects[1].m_id = snd_se_stage_Madein_04;
    this->m_soundEffects[1].m_repeatFrame = 0;
    this->m_soundEffects[1].m_nodeIndex = 0;
    this->m_soundEffects[1].m_endFrame = 0;
    this->m_soundEffects[1].m_offsetPos = Vec2f(0.0, 0.0);
    this->m_soundEffects[2].m_id = snd_se_stage_Madein_02;
    this->m_soundEffects[2].m_repeatFrame = 0;
    this->m_soundEffects[2].m_nodeIndex = 0;
    this->m_soundEffects[2].m_endFrame = 0;
    this->m_soundEffects[2].m_offsetPos = Vec2f(0.0, 0.0);
    this->m_soundEffects[3].m_id = snd_se_stage_Madein_03;
    this->m_soundEffects[3].m_repeatFrame = 0;
    this->m_soundEffects[3].m_nodeIndex = 0;
    this->m_soundEffects[3].m_endFrame = 0;
    this->m_soundEffects[3].m_offsetPos = Vec2f(0.0, 0.0);
    this->m_soundEffects[4].m_id = snd_se_stage_Madein_05;
    this->m_soundEffects[4].m_repeatFrame = 0;
    this->m_soundEffects[4].m_nodeIndex = 0;
    this->m_soundEffects[4].m_endFrame = 0;
    this->m_soundEffects[4].m_offsetPos = Vec2f(0.0, 0.0);
    this->m_soundEffects[5].m_id = snd_se_stage_Madein_06;
    this->m_soundEffects[5].m_repeatFrame = 0;
    this->m_soundEffects[5].m_nodeIndex = 0;
    this->m_soundEffects[5].m_endFrame = 0;
    this->m_soundEffects[5].m_offsetPos = Vec2f(0.0, 0.0);

}

void grTempleOfTimeDoor::update(float deltaFrame) {

    switch(this->state) {
        case State_Closed:
            if (this->checkForPinch()) {
                this->startGimmickSE(2);
                this->setMotion(State_Open);
                this->state = State_Open;
            }
            else if (this->checkForOcarina()){
                this->openedWithOcarina = true;
                this->startGimmickSE(2);
                this->setMotion(State_Open);
                this->state = State_Open;
            }
            break;
        case State_Open:
            if ((this->m_modelAnims[0])->getFrame() == 250) {
                this->startGimmickSE(3);
            }

            if ((this->m_modelAnims[0])->getFrame() >= (this->m_modelAnims[0])->getFrameCount()) {
                this->setMotion(State_Opened);
                this->state = State_Opened;
                this->startGimmickSE(4);
                if (this->openedWithOcarina) {
                    this->startGimmickSE(5);
                }
            }
            break;
        default:
            break;
    }

}

void grTempleOfTimeDoor::playOcarinaSong() {
    this->startGimmickSE(0);
    this->startGimmickSE(1);
}

