#pragma once

#include <gr/gr_madein.h>
#include <so/so_world.h>
#include "st_lunarhorizon_data.h"

class grLunarHorizonColour : public grMadein {
protected:
    int type;
    float consecutiveFrames;
    float turnOffTimer;
    float* cooldownTimerWork;
    bool isActive;
    grGimmickMotionPathData motionPathData;
public:
    grLunarHorizonColour(const char* taskName) : grMadein(taskName)
    {
        turnOffTimer = 0.0;
        consecutiveFrames = 0.0;
        isActive = false;
    };
    virtual void startup(gfArchive* archive, u32 unk1, gfSceneRoot::LayerType layerType);
    virtual void update(float deltaFrame);
    virtual ~grLunarHorizonColour(){};
    virtual void receiveCollMsg_Landing(grCollStatus* collStatus, grCollisionJoint* collisionJoint, bool unk3);

    virtual void setMotionPathData(int mdlIndex);
    virtual void setType(int type);
    virtual void setCooldownWork(float* cooldownTimerWork);
    virtual void reverseType();

    static grLunarHorizonColour* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
