#pragma once

#include "gr_templeoftime.h"
#include <memory.h>
#include <st/stage.h>
#include "st_templeoftime.h"

class grTempleOfTimeMedallion : public grTempleOfTime {

protected:
    stTempleOfTime* stage;

    enum State {
        State_Closed = 0x0,
        State_Open = 0x1,
        State_Opened = 0x2,
    };

    State state;
    OcarinaSong<6> preludeOfLight;
    stParam* stageParam;

public:
    grTempleOfTimeMedallion(const char* taskName) : grTempleOfTime(taskName)
    {
        state = State_Closed;
        preludeOfLight.buttons[0].m_appealHi = true;
        preludeOfLight.buttons[1].m_appealSR = true;
        preludeOfLight.buttons[2].m_appealHi = true;
        preludeOfLight.buttons[3].m_appealSR = true;
        preludeOfLight.buttons[4].m_appealSL = true;
        preludeOfLight.buttons[5].m_appealHi = true;
        ocarinaSong = &preludeOfLight;
    };
    virtual void update(float deltaFrame);
    virtual void startup(gfArchive* archive, u32 unk1, gfSceneRoot::LayerType layerType);
    virtual ~grTempleOfTimeMedallion(){};

    virtual void playOcarinaSong();
    virtual void setStageParam(stParam* stpm);

    static grTempleOfTimeMedallion* create(int mdlIndex, const char* tgtNodeName, const char* taskName, stTempleOfTime* stage);
};
