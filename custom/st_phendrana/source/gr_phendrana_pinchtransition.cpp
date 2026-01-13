#include <memory.h>
#include <ec/ec_mgr.h>
#include "gr_phendrana_pinchtransition.h"
#include <sc/sc_melee.h>
#include <ft/ft_manager.h>
#include <gm/gm_global_mode_melee.h>
#include <OS/OSError.h>

grPhendranaPinchTransition* grPhendranaPinchTransition::create(int mdlIndex, const char* taskName, stPhendrana* stage, bool isRidleyNode)
{
    grPhendranaPinchTransition* phendranaPinch = new (Heaps::StageInstance) grPhendranaPinchTransition(taskName);
    phendranaPinch->setup(mdlIndex, taskName, stage, isRidleyNode);

    return phendranaPinch;
}

void grPhendranaPinchTransition::startup(gfArchive* archive, u32 unk1, gfSceneRoot::LayerType layerType)
{
    grPhendranaItem::startup(archive, unk1, layerType);

    this->changeState(State_Start);
}

void grPhendranaPinchTransition::update(float deltaFrame)
{
    grPhendranaPinch::update(deltaFrame);
    
    if (this->state == State_Transition) {
        bool endTransition = false;
        switch (this->transitionCheck) {
            case TransitionCheck_CHR:
                endTransition = this->m_modelAnims[0]->m_anmObjChrRes->GetFrame() >= this->m_modelAnims[0]->m_anmObjChrRes->m_anmChrFile->m_animLength - 1;
                break;
            case TransitionCheck_SRT:
                endTransition = this->m_modelAnims[0]->m_anmObjTexSrtRes->GetFrame() >= this->m_modelAnims[0]->m_anmObjTexSrtRes->m_anmTexSrtFile->m_animLength - 1;
                break;
            case TransitionCheck_SHP:
                endTransition = this->m_modelAnims[0]->m_anmObjShpRes->GetFrame() >= this->m_modelAnims[0]->m_anmObjShpRes->m_anmShpFile->m_animLength - 1;
                break;
            case TransitionCheck_PAT:
                endTransition = this->m_modelAnims[0]->m_anmObjTexPatRes->GetFrame() >= this->m_modelAnims[0]->m_anmObjTexPatRes->m_anmTexPatFile->m_animLength - 1;
                break;
            case TransitionCheck_VIS:
                endTransition = this->m_modelAnims[0]->m_anmObjVisRes->GetFrame() >= this->m_modelAnims[0]->m_anmObjVisRes->m_anmVisFile->m_animLength - 1;
                break;
            case TransitionCheck_CLR:
                endTransition = this->m_modelAnims[0]->m_anmObjMatClrRes->GetFrame() >= this->m_modelAnims[0]->m_anmObjMatClrRes->m_anmMatClrFile->m_animLength - 1;
                break;
            default:
                endTransition = this->m_modelAnims[0]->getFrame() >= this->m_modelAnims[0]->getFrameCount() - 1;
                break;
        }
        if (endTransition) {
            this->changeState(State_End);
        }
    }
}

void grPhendranaPinchTransition::activatePinch() {
    this->isActivated = true;
    if (this->m_gimmickMotionPath != NULL) {
        this->m_gimmickMotionPath->startMove();
    }
    this->changeState(State_Transition);
}

void grPhendranaPinchTransition::changeState(State state) {
    float longestFrameCount = 0; // TODO once it becomes possible to check if an animation type is loop enabled
    TransitionCheck transitionCheck = TransitionCheck_ANY; // TODO same

    float lastStateChrFrame = -1;
    float lastStateSrtFrame = -1;
    float lastStateShpFrame = -1;
    float lastStatePatFrame = -1;
    float lastStateVisFrame = -1;
    float lastStateClrFrame = -1;

    if (lastStateChrEnabled && this->m_resFile.GetResAnmChrNumEntries() <= state) {
        lastStateChrFrame = this->m_modelAnims[0]->m_anmObjChrRes->GetFrame();
    }
    if (lastStateSrtEnabled && this->m_resFile.GetResAnmTexSrtNumEntries() <= state) {
        lastStateSrtFrame = this->m_modelAnims[0]->m_anmObjTexSrtRes->GetFrame();
    }
    if (lastStateShpEnabled && this->m_resFile.GetResAnmShpNumEntries() <= state) {
        lastStateShpFrame = this->m_modelAnims[0]->m_anmObjShpRes->GetFrame();
    }
    if (lastStatePatEnabled && this->m_resFile.GetResAnmTexPatNumEntries() <= state) {
        lastStatePatFrame = this->m_modelAnims[0]->m_anmObjTexPatRes->GetFrame();
    }
    if (lastStateVisEnabled && this->m_resFile.GetResAnmVisNumEntries() <= state) {
        lastStateVisFrame = this->m_modelAnims[0]->m_anmObjVisRes->GetFrame();
    }
    if (lastStateClrEnabled && this->m_resFile.GetResAnmClrNumEntries() <= state) {
        lastStateClrFrame = this->m_modelAnims[0]->m_anmObjMatClrRes->GetFrame();
    }

    if (this->m_resFile.GetResAnmChrNumEntries() > state) {
        this->changeNodeAnim(state, 0);
        lastStateChrEnabled = true;
    }
    if (this->m_resFile.GetResAnmTexSrtNumEntries() > state) {
        this->changeTexSrtAnim(state, 0);
        lastStateSrtEnabled = true;
    }
    if (this->m_resFile.GetResAnmShpNumEntries() > state) {
        this->changeShapeAnim(state, 0);
        lastStateShpEnabled = true;
    }
    if (this->m_resFile.GetResAnmTexPatNumEntries() > state) {
        this->changeTexAnim(state, 0);
        lastStatePatEnabled = true;
    }
    if (this->m_resFile.GetResAnmVisNumEntries() > state) {
        this->changeVisibleAnim(state, 0);
        lastStateVisEnabled = true;
    }
    if (this->m_resFile.GetResAnmClrNumEntries() > state) {
        this->changeMatColAnim(state, 0);
        lastStateClrEnabled = true;
    }

    if (lastStateChrFrame > -1) {
        this->m_modelAnims[0]->m_anmObjChrRes->SetFrame(lastStateChrFrame);
    }
    if (lastStateSrtFrame > -1) {
        this->m_modelAnims[0]->m_anmObjTexSrtRes->SetFrame(lastStateSrtFrame);
    }
    if (lastStateShpFrame > -1) {
        this->m_modelAnims[0]->m_anmObjShpRes->SetFrame(lastStateShpFrame);
    }
    if (lastStatePatFrame > -1) {
        this->m_modelAnims[0]->m_anmObjTexPatRes->SetFrame(lastStatePatFrame);
    }
    if (lastStateVisFrame > -1) {
        this->m_modelAnims[0]->m_anmObjVisRes->SetFrame(lastStateVisFrame);
    }
    if (lastStateClrFrame > -1) {
        this->m_modelAnims[0]->m_anmObjMatClrRes->SetFrame(lastStateClrFrame);
    }

    this->state = state;
}