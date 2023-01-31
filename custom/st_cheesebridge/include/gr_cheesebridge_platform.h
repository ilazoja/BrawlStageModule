#pragma once

#include <gr/gr_madein.h>
#include <gr/gr_gimmick_motion_path.h>
#include <mt/mt_prng.h>
#include "st_cheesebridge_data.h"

class grCheeseBridgePlatform : public grMadein {
protected:
    grGimmickMotionPathData motionPathData;
    RailObjectParam* railParam;
    float respawnTimer;
    float* cooldownTimerWork;
public:
    grCheeseBridgePlatform(char* taskName) : grMadein(taskName)
    {
        respawnTimer = 0.0;
    };
    virtual ~grCheeseBridgePlatform(){};
    virtual void startup(gfArchive* archive, u32 unk1, u32 unk2);
    virtual void update(float deltaFrame);

    virtual void setMotionPathData(int mdlIndex);
    virtual void setCooldownTimerWork(float* cooldownTimerWork);
    virtual void setOrientation();

    static grCheeseBridgePlatform* create(int mdlIndex, char* tgtNodeName, char* taskName);
};
