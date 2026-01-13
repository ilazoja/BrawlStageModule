#include <memory.h>
#include <ec/ec_mgr.h>
#include <cstdlib>
#include "gr_phendrana_flickerbat.h"
#include <OS/OSError.h>

grPhendranaFlickerbat* grPhendranaFlickerbat::create(int mdlIndex, const char* taskName, stPhendrana* stage, bool isRidleyNode)
{
    grPhendranaFlickerbat* phendranaFlickerbat = new (Heaps::StageInstance) grPhendranaFlickerbat(taskName);
    phendranaFlickerbat->setup(mdlIndex, taskName, stage, isRidleyNode);

    return phendranaFlickerbat;
}

void grPhendranaFlickerbat::startup(gfArchive* archive, u32 unk1, gfSceneRoot::LayerType layerType)
{
    grPhendranaOther::startup(archive, unk1, layerType);

    if (this->soundEffectIndex == 0) {
        this->createSoundWork(2, 1);
    
        this->m_soundEffects[0].m_id = snd_se_stage_Pirates_18;
        this->m_soundEffects[0].m_repeatFrame = 0;
        this->m_soundEffects[0].m_nodeIndex = this->getNodeIndex(0, "Effect");
        this->m_soundEffects[0].m_endFrame = 0;
        this->m_soundEffects[0].m_offsetPos = Vec2f(0.0, 0.0);
    
        this->m_soundEffects[1].m_id = snd_se_stage_Pirates_19;
        this->m_soundEffects[1].m_repeatFrame = 0;
        this->m_soundEffects[1].m_nodeIndex = this->getNodeIndex(0, "Effect");
        this->m_soundEffects[1].m_endFrame = 0;
        this->m_soundEffects[1].m_offsetPos = Vec2f(0.0, 0.0);
    }

    this->changeState(State_Fly);
}

void grPhendranaFlickerbat::update(float deltaFrame)
{
    grPhendranaOther::update(deltaFrame);

    int chance = 0;
    if (this->framesCounter >= 180) {
        chance = rand() % 500;

        if (chance == 0) {
            if (this->soundEffectIndex == 0) {
                chance = rand() % 2;
                if (chance == 0) {
                    this->startGimmickSE(0);
                }
                else {
                    this->startGimmickSE(1);
                }
            }
            else {
                this->startGimmickSE(0);
            }

            this->framesCounter = 0;
        }
    }
    else {
        if (this->lastMotionFrame != this->m_gimmickMotionPath->getFrame()) {
            this->lastMotionFrame = this->m_gimmickMotionPath->getFrame();
            this->framesCounter++;
        }
    }

    if (this->m_modelAnims[0]->getFrame() >= this->m_modelAnims[0]->getFrameCount() - 1) {
        switch(this->state) {
            case State_Fly:
                chance = rand() % 10;
                if (chance == 0) {
                    this->changeState(State_GlideStart);
                }
                break;
            case State_GlideStart:
                this->changeState(State_GlideLoop);
                break;
            case State_GlideLoop:
                chance = rand() % 5;
                if (chance >= 3) {
                    this->changeState(State_Spin);
                }
                else if (chance >= 1) {
                    this->changeState(State_GlideEnd);
                }
                break;
            case State_GlideEnd:
                this->changeState(State_Fly);
                break;
            case State_Spin:
                chance = rand() % 11;
                if (chance == 10) {
                    this->changeState(State_GlideLoop);
                }
                else {
                    this->changeState(State_GlideEnd);
                }
                break;
            default:
                break;
        }
    }
}

void grPhendranaFlickerbat::changeState(State state) {
    this->changeNodeAnim(state, 0);
    this->changeMatColAnim(state, 0);
    this->changeVisibleAnim(state, 0);
    this->state = state;
}