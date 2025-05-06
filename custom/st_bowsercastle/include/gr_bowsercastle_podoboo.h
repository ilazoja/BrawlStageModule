#pragma once

#include <gr/gr_madein.h>
#include "st_bowsercastle_data.h"

class grBowserCastlePodoboo : public grMadein
{

        enum State {
            State_Inactive = -0x1,
            State_Wait = 0x0,
            State_Hop = 0x1,
            State_Sink = 0x2,
        };

        State state;
        float timer;

    public:


        grBowserCastlePodoboo(const char* taskName) : grMadein(taskName) {
            state = State_Inactive;
        }
        virtual void startup(gfArchive* archive, u32 unk1, u32 unk2);
        virtual void update(float deltaFrame);
        virtual ~grBowserCastlePodoboo() { };
        virtual void changeState(State state);

        static grBowserCastlePodoboo* create(int mdlIndex, const char* tgtNodeName, const char* taskName);

};
