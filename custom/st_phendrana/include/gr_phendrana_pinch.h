#pragma once

#include "gr_phendrana_item.h"
#include <st/st_melee.h>

class grPhendranaPinch : public grPhendranaItem
{
    protected:
        bool isActivated;
        bool isFinished;

    public:
        grPhendranaPinch(const char* taskName) : grPhendranaItem(taskName) {
            isActivated = false;
            isFinished = false;
        };
        virtual ~grPhendranaPinch() { };
        
        virtual void startup(gfArchive* archive, u32 unk1, gfSceneRoot::LayerType layerType);
        virtual void update(float deltaFrame);

        virtual void setMotionPathData(int mdlIndex, bool isRotateMotionPath, u8 effectIndex = 0, int soundEffectIndex = 0);

        virtual bool checkForPinch();
        virtual void activatePinch();

        static grPhendranaPinch* create(int mdlIndex, const char* taskName, stPhendrana* stage, bool isRidleyNode);
};