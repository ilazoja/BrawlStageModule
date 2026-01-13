#pragma once

#include <gr/gr_madein.h>
#include <so/so_world.h>
#include "st_lunarhorizon_data.h"

class grLunarHorizonYellow : public grMadein {
protected:
    float turnOffTimer;
    bool isActive;
    float* cooldownTimerWork;
    bool* isResetWork;
public:

    grLunarHorizonYellow(const char* taskName) : grMadein(taskName)
    {
        turnOffTimer = 0.0;
        isActive = false;
    };
    virtual void startup(gfArchive* archive, u32 unk1, gfSceneRoot::LayerType layerType);
    virtual void update(float deltaFrame);
    virtual ~grLunarHorizonYellow(){};
    virtual void receiveCollMsg_Landing(grCollStatus* collStatus, grCollisionJoint* collisionJoint, bool unk3);

    virtual void setCooldownWork(float* cooldownTimerWork);
    virtual void setIsResetWork(bool* isResetWork);

    static grLunarHorizonYellow* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
