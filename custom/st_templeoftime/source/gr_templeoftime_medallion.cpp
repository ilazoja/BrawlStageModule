#include "gr_templeoftime_medallion.h"
#include <ec/ec_mgr.h>
#include <memory.h>
#include <ft/ft_manager.h>
#include <OS/OSError.h>
#include <sc/sc_melee.h>
#include <snd/snd_system.h>

grTempleOfTimeMedallion* grTempleOfTimeMedallion::create(int mdlIndex, const char* tgtNodeName, const char* taskName, stTempleOfTime* stage)
{
    grTempleOfTimeMedallion* ground = new (Heaps::StageInstance) grTempleOfTimeMedallion(taskName);
    ground->stage = stage;
    ground->setupMelee();
    ground->setMdlIndex(mdlIndex);
    ground->m_heapType = Heaps::StageInstance;
    ground->makeCalcuCallback(1, Heaps::StageInstance);
    ground->setCalcuCallbackRoot(7);
    return ground;
}

void grTempleOfTimeMedallion::startup(gfArchive* archive, u32 unk1, gfSceneRoot::LayerType layerType) {
    grMadein::startup(archive, unk1, layerType);
    this->createSoundWork(4,1);
    this->m_soundEffects[0].m_id = snd_se_stage_Madein_01;
    this->m_soundEffects[0].m_repeatFrame = 0;
    this->m_soundEffects[0].m_nodeIndex = 0;
    this->m_soundEffects[0].m_endFrame = 0;
    this->m_soundEffects[0].m_offsetPos = Vec2f(0.0, 0.0);
    this->m_soundEffects[1].m_id = snd_se_stage_Madein_Caution;
    this->m_soundEffects[1].m_repeatFrame = 0;
    this->m_soundEffects[1].m_nodeIndex = 0;
    this->m_soundEffects[1].m_endFrame = 0;
    this->m_soundEffects[1].m_offsetPos = Vec2f(0.0, 0.0);
    this->m_soundEffects[2].m_id = snd_se_stage_Madein_07;
    this->m_soundEffects[2].m_repeatFrame = 0;
    this->m_soundEffects[2].m_nodeIndex = 0;
    this->m_soundEffects[2].m_endFrame = 0;
    this->m_soundEffects[2].m_offsetPos = Vec2f(0.0, 0.0);
    this->m_soundEffects[3].m_id = snd_se_stage_Madein_Arrow;
    this->m_soundEffects[3].m_repeatFrame = 0;
    this->m_soundEffects[3].m_nodeIndex = 0;
    this->m_soundEffects[3].m_endFrame = 0;
    this->m_soundEffects[3].m_offsetPos = Vec2f(0.0, 0.0);
}

void grTempleOfTimeMedallion::update(float deltaFrame) {

    switch(this->state) {
        case State_Closed:
            if (this->checkForOcarina()){
                this->state = State_Open;
                this->startGimmickSE(2);
                this->setMotion(State_Open);
                this->stage->changeScnAnim(1);
                this->state = State_Open;
            }
            break;
        case State_Open:
            if ((this->m_modelAnims[0])->getFrame() >= (this->m_modelAnims[0])->getFrameCount()) {
                g_sndSystem->setBGMVol(true, static_cast<float>(this->stageParam->m_bgmVolume) * 0.01);
                this->state = State_Opened;
            }
        default:
            break;
    }

}

void grTempleOfTimeMedallion::playOcarinaSong() {
    g_sndSystem->setBGMVol(true, 0);
    this->startGimmickSE(0);
    this->startGimmickSE(1);
}

void grTempleOfTimeMedallion::setStageParam(stParam* stageParam) {
    this->stageParam = stageParam;
}

// TODO: Change lights

