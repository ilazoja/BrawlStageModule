#pragma once

#include <gr/gr_madein.h>
#include <st/st_melee.h>

class grGhostHouseHole : public grMadein {
protected:
    enum State {
        State_Invalid = -0x1,
        State_Inactive = 0x0,
        State_Appear = 0x1,
        State_Active = 0x2,
        State_Disappear = 0x3
    };

    grGimmickMotionPathData motionPathData;
    State state;
    float timer;

public:
    grGhostHouseHole(const char* taskName) : grMadein(taskName)
    {
        state = State_Invalid;
    };
    virtual void update(float deltaFrame);
    virtual void startup(gfArchive* archive, u32 unk1, gfSceneRoot::LayerType layerType);
    virtual ~grGhostHouseHole(){};
    virtual void updateColl(float deltaFrame);
    virtual void setMotionPathData(int mdlIndex);
    virtual void changeState(State state);
    virtual void updateStatus(float deltaFrame);

    static grGhostHouseHole* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
