#include <memory.h>
#include <ec/ec_mgr.h>
#include <mt/mt_prng.h>
#include "gr_bowsercastle_podoboo.h"
#include <OS/OSError.h>

grBowserCastlePodoboo* grBowserCastlePodoboo::create(int mdlIndex, const char* tgtNodeName, const char* taskName){
    grBowserCastlePodoboo* podoboo = new(Heaps::StageInstance) grBowserCastlePodoboo(taskName);
    podoboo->setupMelee();
    podoboo->setMdlIndex(mdlIndex);
    podoboo->m_heapType = Heaps::StageInstance;
    podoboo->makeCalcuCallback(1, Heaps::StageInstance);
    podoboo->setCalcuCallbackRoot(7);

    return podoboo;
}

void grBowserCastlePodoboo::startup(gfArchive* archive, u32 unk1, gfSceneRoot::LayerType layerType) {
    grMadein::startup(archive, unk1, layerType);

    this->createSoundWork(kNumPodobooSfxVariation*2,1);
    for (int i = 0; i < kNumPodobooSfxVariation; i++) {
        this->m_soundEffects[i].m_id = snd_se_stage_BowserCastle_Podoboo_Hop + i;
        this->m_soundEffects[i].m_repeatFrame = 0;
        this->m_soundEffects[i].m_nodeIndex = 0;
        this->m_soundEffects[i].m_endFrame = 0;
        this->m_soundEffects[i].m_offsetPos = Vec2f(0.0, 0.0);
    }
    for (int i = 0; i < kNumPodobooSfxVariation; i++) {
        this->m_soundEffects[i + kNumPodobooSfxVariation].m_id = snd_se_stage_BowserCastle_Podoboo_Sink + i;
        this->m_soundEffects[i + kNumPodobooSfxVariation].m_repeatFrame = 0;
        this->m_soundEffects[i + kNumPodobooSfxVariation].m_nodeIndex = 0;
        this->m_soundEffects[i + kNumPodobooSfxVariation].m_endFrame = 0;
        this->m_soundEffects[i + kNumPodobooSfxVariation].m_offsetPos = Vec2f(0.0, 0.0);
    }

    this->setMotionRatio(0.0);
    this->changeState(State_Wait);
}


void grBowserCastlePodoboo::update(float deltaFrame){
    grMadein::update(deltaFrame);

    stBowserCastleData *stageData = static_cast<stBowserCastleData *>(this->getStageData());

    switch(this->state) {
        case State_Wait:
            if (this->timer > 0) {
                this->timer -= deltaFrame;
                if (this->timer <= 0) {
                    this->changeState(State_Hop);
                }
            }
            break;
        case State_Hop:
            if ((this->m_modelAnims[0])->getFrame() >= stageData->podobooSinkFrame) {
                this->changeState(State_Sink);
            }
            break;
        case State_Sink:
            if ((this->m_modelAnims[0])->getFrame() >= (this->m_modelAnims[0])->getFrameCount()) {
                this->changeState(State_Wait);
            }
            break;
        default:
            break;
    }
}

void grBowserCastlePodoboo::changeState(State state) {
    stBowserCastleData *stageData = static_cast<stBowserCastleData *>(this->getStageData());

    if (this->state != state) {
        switch(state) {
            case State_Wait:
                this->timer = randi(stageData->podobooMaxRespawnFrames - stageData->podobooMinRespawnFrames) + stageData->podobooMinRespawnFrames;
                break;
            case State_Hop:
                this->setMotionRatio(1.0);
                this->setMotionFrame(0, 0);
                this->startGimmickSE(randi(kNumPodobooSfxVariation));
                break;
            case State_Sink:
                this->startGimmickSE(randi(kNumPodobooSfxVariation) + kNumPodobooSfxVariation);
                break;
            default:
                break;
        }

        this->state = state;
    }
}
