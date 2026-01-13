#pragma once

#include "gr_phendrana_other.h"
#include <st/st_melee.h>

class grPhendranaFlickerbat : public grPhendranaOther
{
    protected:
        enum State {
            State_Fly = 0x0,
            State_GlideStart = 0x1,
            State_GlideLoop = 0x2,
            State_GlideEnd = 0x3,
            State_Spin = 0x4
        };

        State state;
        int lastMotionFrame;
        int framesCounter;

        virtual void changeState(State state);

    public:
        grPhendranaFlickerbat(const char* taskName) : grPhendranaOther(taskName) {
            state = State_Fly;
            lastMotionFrame = 0;
            framesCounter = 0;
        };
        virtual ~grPhendranaFlickerbat() { };

        virtual void startup(gfArchive* archive, u32 unk1, gfSceneRoot::LayerType layerType);
        virtual void update(float deltaFrame);

        static grPhendranaFlickerbat* create(int mdlIndex, const char* taskName, stPhendrana* stage, bool isRidleyNode);
};