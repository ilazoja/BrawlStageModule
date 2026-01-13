#pragma once

#include "gr_phendrana_pinch.h"
#include <st/st_melee.h>

class grPhendranaPinchTransition : public grPhendranaPinch
{
    protected:
        enum State {
            State_Start = 0x0,
            State_Transition = 0x1,
            State_End = 0x2
        };

        enum TransitionCheck {
            TransitionCheck_CHR = 0x0,
            TransitionCheck_SRT = 0x1,
            TransitionCheck_SHP = 0x2,
            TransitionCheck_PAT = 0x3,
            TransitionCheck_VIS = 0x4,
            TransitionCheck_CLR = 0x5,
            TransitionCheck_ANY = 0x6
        };

        State state;
        TransitionCheck transitionCheck;
        bool lastStateChrEnabled;
        bool lastStateSrtEnabled;
        bool lastStateShpEnabled;
        bool lastStatePatEnabled;
        bool lastStateVisEnabled;
        bool lastStateClrEnabled;

        virtual void changeState(State state);

    public:
        grPhendranaPinchTransition(const char* taskName) : grPhendranaPinch(taskName) {
            state = State_Start;
            lastStateChrEnabled = false;
            lastStateSrtEnabled = false;
            lastStateShpEnabled = false;
            lastStatePatEnabled = false;
            lastStateVisEnabled = false;
            lastStateClrEnabled = false;
            transitionCheck = TransitionCheck_ANY;
        };
        virtual ~grPhendranaPinchTransition() { };
        
        virtual void startup(gfArchive* archive, u32 unk1, gfSceneRoot::LayerType layerType);
        virtual void update(float deltaFrame);

        virtual void activatePinch();

        static grPhendranaPinchTransition* create(int mdlIndex, const char* taskName, stPhendrana* stage, bool isRidleyNode);
};