#pragma once

#include "gr_phendrana_pinch.h"
#include "gr_phendrana_ridleysfx.h"
#include <st/st_melee.h>

class grPhendranaRidley : public grPhendranaPinch
{
    protected:
        enum State {
            State_Fly = 0x0,
            State_Roar = 0x1
        };

        bool hasRoared;
        State state;
        grPhendranaRidleySfx* phendranaRidleySfx;

        virtual void changeState(State state);
    
    public:
        grPhendranaRidley(const char* taskName) : grPhendranaPinch(taskName) {
            hasRoared = false;
            state = State_Fly;
        };
        virtual ~grPhendranaRidley() { };

        virtual void startup(gfArchive* archive, u32 unk1, gfSceneRoot::LayerType layerType);
        virtual void update(float deltaFrame);

        static grPhendranaRidley* create(int mdlIndex, const char* taskName, stPhendrana* stage, bool isRidleyNode, grPhendranaRidleySfx* phendranaRidleySfx);
        virtual void setup(int mdlIndex, const char* taskName, stPhendrana* stage, bool isRidleyNode, grPhendranaRidleySfx* phendranaRidleySfx);
};