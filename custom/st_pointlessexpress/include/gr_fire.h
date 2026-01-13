#pragma once

#include <gr/gr_madein.h>

struct grFireData {
    u32 addedShieldDamage;
    int reactionEffect;
    int reactionFix;
    int reactionAdd;
    int power;
    float hitstopMultiplier;
    int detectionRate;
};

class grFire : public grMadein {
protected:
    grGimmickMotionPathData motionPathData;
    int vector;
    float size;
    bool isCapsule;

public:
    grFire(const char* taskName) : grMadein(taskName)
    {

    };
    virtual void startup(gfArchive* archive, u32 unk1, gfSceneRoot::LayerType layerType);
    virtual ~grFire(){};
    virtual void setMotionPathData(int mdlIndex);
    virtual void setSizeAndVector(float size, int vector, bool isCapsule);

    static grFire* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
