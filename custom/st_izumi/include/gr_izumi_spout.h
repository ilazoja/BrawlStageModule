#pragma once

#include <gr/gr_yakumono.h>
#include <gm/gm_global.h>

class grIzumiSpout : public grYakumono {
protected:

    enum Level {
        Level_Invalid = -1,
        Level_Off = 0,
        Level_Active = 1,
        Level_Sink = 2,
    };

    u32 spoutId;
    bool isForceDown;
    bool isItemObtained;
    bool isActive;
    float effFrameCount;
    u32 effPtr;
    Level level;
    float spoutTimer;

    snd3DGenerator soundGenerator;
    int sndPtr;

    soAreaData areaData;
    soSet<soAreaData> areaInit;
    ykAreaData areaInfo;
    int anmChrIndex;
    int anmSrtIndex;
    int anmVisIndex;
    int anmClrIndex;

    float destFrame;

public:
    grIzumiSpout(const char* taskName) : grYakumono(taskName)
    {
        anmChrIndex = -1;
        isForceDown = false;
        isItemObtained = false;
    };
    virtual void processAnim();
    virtual void update(float deltaFrame);
    virtual ~grIzumiSpout(){};
    virtual void onGimmickEvent(soGimmickEventArgs* eventInfo, int* taskId);
    virtual void receiveCollMsg_Landing(grCollStatus* collStatus, grCollisionJoint* collisionJoint, bool unk3);

    virtual void fountainInit(u32 spoutId);
    virtual void startFountainEffect();
    virtual void stopFountainEffect();
    virtual void updateEff(float deltaFrame);
    virtual void updateColl(float deltaFrame);
    virtual void updateLevel(float deltaFrame);
    virtual void changeDestFrame();
    virtual void setFrame(float frame);
    virtual void setMotion(u32 chrIndex = -1, u32 srtIndex = -1, u32 visIndex = -1, u32 matColIndex = -1);

    static grIzumiSpout* create(int mdlIndex, const char* tgtNodeName, const char* taskName);

};