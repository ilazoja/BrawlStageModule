#include "gr_elevate.h"
#include <ec/ec_mgr.h>
#include <memory.h>
#include <mt/mt_prng.h>

grElevate* grElevate::create(int mdlIndex, const char* tgtNodeName, const char* taskName)
{
    grElevate* elevate = new (Heaps::StageInstance) grElevate(taskName);
    elevate->setMdlIndex(mdlIndex);
    elevate->m_heapType = Heaps::StageInstance;
    elevate->makeCalcuCallback(1, Heaps::StageInstance);
    elevate->setCalcuCallbackRoot(7);
    elevate->setupMelee();

    return elevate;
}

void grElevate::startup(gfArchive* archive, u32 unk1, u32 unk2)
{
    grMadein::startup(archive, unk1, unk2);
    grFinkratData* finkratData = static_cast<grFinkratData*>(this->getStageData());
    this->elevateTimer = finkratData->platformElevateFrames;

    this->createEffectWork(1);
    this->m_effects[0].m_id = 0x4F0001;
    this->m_effects[0].m_repeatFrame = 0;
    this->m_effects[0].m_nodeIndex = this->getNodeIndex(0, "effect_locator");
    this->m_effects[0].m_endFrame = 0.0;
    this->m_effects[0].m_offsetPos = Vec2f(0.0, 0.0);
    this->m_effects[0].m_scale = 0.0;

    this->createSoundWork(2,1);
    this->m_soundEffects[0].m_id = snd_se_stage_Ice_IceFloor_01;
    this->m_soundEffects[0].m_repeatFrame = 0;
    this->m_soundEffects[0].m_nodeIndex = 0;
    this->m_soundEffects[0].m_endFrame = 0;
    this->m_soundEffects[0].m_offsetPos = Vec2f(0.0, 0.0);
    this->m_soundEffects[1].m_id = snd_se_stage_Ice_IceFloor_02;
    this->m_soundEffects[1].m_repeatFrame = 0;
    this->m_soundEffects[1].m_nodeIndex = 0;
    this->m_soundEffects[1].m_endFrame = 0;
    this->m_soundEffects[1].m_offsetPos = Vec2f(0.0, 0.0);
}

void grElevate::update(float deltaFrame)
{
    grFinkratData* finkratData = static_cast<grFinkratData*>(this->getStageData());

    this->elevateTimer -= deltaFrame;
    if (this->elevateTimer < 0) {
        this->elevateTimer = finkratData->platformElevateFrames;
        if (randf() <= finkratData->platformElevateChance) {
            switch(this->state) {
                case State_Up:
                    this->setMotion(State_Down);
                    this->state = State_Down;
                    this->startGimmickSE(1);
                    this->startGimmickEffect(0);
                    break;
                case State_Down:
                    this->setMotion(State_Up);
                    this->state = State_Up;
                    this->startGimmickSE(0);
                    this->startGimmickEffect(0);
                    break;
                default:
                    break;
            }
        }
    }
    grMadein::update(deltaFrame);
}
