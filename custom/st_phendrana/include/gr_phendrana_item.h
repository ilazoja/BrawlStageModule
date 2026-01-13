#pragma once

#include "st_phendrana.h"
#include <gr/gr_madein.h>
#include <st/st_melee.h>

class grPhendranaItem : public grMadein
{
    protected:
        grGimmickMotionPathData motionPathData;
        bool isRotateMotionPath;
        bool isRidleyNode;
        u8 effectIndex;
        int soundEffectIndex;

    public:
        stPhendrana* stage;

        grPhendranaItem(const char* taskName) : grMadein(taskName) {
            isRidleyNode = false;
            effectIndex = 0xFF;
            soundEffectIndex = 0;
        };
        virtual ~grPhendranaItem() { };

        virtual void startup(gfArchive* archive, u32 unk1, gfSceneRoot::LayerType layerType);
        virtual void update(float deltaFrame);

        virtual void setMotionPathData(int mdlIndex, bool isRotateMotionPath, u8 effectIndex = 0, int soundEffectIndex = 0);
        virtual bool checkForFighterRidley();

        static grPhendranaItem* create(int mdlIndex, const char* taskName, stPhendrana* stage, bool isRidleyNode);
        virtual void setup(int mdlIndex, const char* taskName, stPhendrana* stage, bool isRidleyNode);
};