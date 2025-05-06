#pragma once

#include <gr/gr_madein.h>
#include "st_bowsercastle_data.h"

class grBowserCastleShadow : public grMadein
{

    public:
        enum State {
            State_Inactive = -0x1,
            State_Spawn = 0x0,
            State_Sleep = 0x1,
            State_Awake = 0x2,
            State_Shake = 0x3,
            State_Stomp = 0x4,
            State_Break = 0x5,
        };

        State state;

        grBowserCastleShadow(const char* taskName) : grMadein(taskName) {
            state = State_Inactive;
        }
        virtual void update(float deltaFrame);
        virtual ~grBowserCastleShadow() { };
        virtual void changeState(State state);

        static grBowserCastleShadow* create(int mdlIndex, const char* tgtNodeName, const char* taskName);

};
