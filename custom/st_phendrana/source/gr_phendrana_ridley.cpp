#include <memory.h>
#include <ec/ec_mgr.h>
#include "gr_phendrana_ridley.h"
#include <sc/sc_melee.h>
#include <ft/ft_manager.h>
#include <OS/OSError.h>

grPhendranaRidley* grPhendranaRidley::create(int mdlIndex, const char* taskName, stPhendrana* stage, bool isRidleyNode, grPhendranaRidleySfx* phendranaRidleySfx)
{
    grPhendranaRidley* phendranaRidley = new (Heaps::StageInstance) grPhendranaRidley(taskName);
    phendranaRidley->setup(mdlIndex, taskName, stage, isRidleyNode, phendranaRidleySfx);

    return phendranaRidley;
}

void grPhendranaRidley::setup(int mdlIndex, const char* taskName, stPhendrana* stage, bool isRidleyNode, grPhendranaRidleySfx* phendranaRidleySfx)
{
    this->phendranaRidleySfx = phendranaRidleySfx;
    
    grPhendranaPinch::setup(mdlIndex, taskName, stage, isRidleyNode);
}

void grPhendranaRidley::startup(gfArchive* archive, u32 unk1, gfSceneRoot::LayerType layerType)
{
    grPhendranaPinch::startup(archive, unk1, layerType);

    this->changeState(State_Fly);
}

void grPhendranaRidley::update(float deltaFrame)
{
    grPhendranaPinch::update(deltaFrame);
    
    if (this->isActivated && !this->isFinished) {
        switch(this->state) {
            case State_Fly:
                if (this->m_modelAnims[0]->getFrame() == 42) {
                    this->phendranaRidleySfx->playWingFlap();
                }
                if (!this->hasRoared && this->m_gimmickMotionPath->getFrame() > 880 && this->m_modelAnims[0]->getFrame() >= this->m_modelAnims[0]->getFrameCount() - 1) {
                    this->changeState(State_Roar);
                    this->hasRoared = true;
                }
                break;
            case State_Roar:
                if (this->m_modelAnims[0]->getFrame() == 42 || this->m_modelAnims[0]->getFrame() == 84 || this->m_modelAnims[0]->getFrame() == 104) {
                    this->phendranaRidleySfx->playWingFlap();
                }
                if (this->m_modelAnims[0]->getFrame() == 40) {
                    this->phendranaRidleySfx->playRoar();
                }
                if (this->m_modelAnims[0]->getFrame() >= this->m_modelAnims[0]->getFrameCount() - 1) {
                    this->changeState(State_Fly);
                }
                break;
            default:
                break;
        }
        
        if (this->getMotionFrame(0) >= 2199) {
            this->isFinished = true;
        }
    }
}

void grPhendranaRidley::changeState(State state) {
    this->changeNodeAnim(state, 0);
    this->state = state;
}