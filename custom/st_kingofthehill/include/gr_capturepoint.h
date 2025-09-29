#pragma once

#include <gr/gr_madein.h>
#include <ft/ft_manager.h>
#include <st/st_melee.h>
#include "st_kingofthehill_data.h"

class grCapturePoint : public grMadein {
protected:
    enum State {
        State_Off = 0x0,
        State_Appear = 0x1,
        State_On = 0x2,
        State_In = 0x3,
        State_Capturing = 0x4,
        State_Out = 0x5,
        State_Disappear = 0x6
    };

    enum CollisionMode {
        CollisionMode_Off = 0x0,
        CollisionMode_On = 0x1,
        CollisionMode_CaptureOnly = 0x2
    };

    grGimmickMotionPathData motionPathData;
    soAreaData areaData;
    soSet<soAreaData> areaInit;
    ykAreaData areaInfo;
    stMelee* stage;
    Ground* capturePointPositions;
    grMadein* syncedGround;
    int syncedGroundExitAnim;
    int selectedNodeIndex;
    float consecutiveFramesCaptured;
    u32 numCaptures;
    u32 targetNumCaptures;
    float stayCapturedTimer;
    int bonusMultiplier;
    State state : 8;
    GameRule gameRule : 8;
    CollisionMode collisionMode : 8;

public:
    grCapturePoint(const char* taskName) : grMadein(taskName)
    {
        syncedGround = NULL;
        syncedGroundExitAnim = -1;
        numCaptures = 0;
        selectedNodeIndex = -1;
        state = State_Off;
        stayCapturedTimer = 0.0;

    };
    virtual void startup(gfArchive* archive, u32 unk1, u32 unk2);
    virtual void update(float deltaFrame);
    virtual void onGimmickEvent(soGimmickEventArgs* eventInfo, int* taskId);
    virtual ~grCapturePoint(){};
    virtual void setCapturePointPositions(Ground* capturePointPositions);
    virtual void setGameRule(GameRule gameRule);
    virtual void setNewCapturePosition();
    virtual void applyMotionRate(float motionRate);

    static grCapturePoint* create(int mdlIndex, const char* tgtNodeName, const char* taskName, stMelee* stage);
};
