#pragma once

#include "gr_phendrana_pinch.h"
#include <st/st_melee.h>

class grPhendranaRidleySfx : public grPhendranaPinch
{    
    public:
        grPhendranaRidleySfx(const char* taskName) : grPhendranaPinch(taskName) {};
        virtual ~grPhendranaRidleySfx() { };

        virtual void startup(gfArchive* archive, u32 unk1, gfSceneRoot::LayerType layerType);

        virtual void playWingFlap();
        virtual void playRoar();

        static grPhendranaRidleySfx* create(int mdlIndex, const char* taskName, stPhendrana* stage, bool isRidleyNode);
};